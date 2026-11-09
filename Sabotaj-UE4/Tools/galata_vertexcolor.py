import bpy, sys, os
from mathutils import Vector
import numpy as np
argv = sys.argv[sys.argv.index("--") + 1:]
usd, fbx = argv[0], argv[1]
R = float(argv[2]) if len(argv) > 2 else 60.0

bpy.ops.wm.read_factory_settings(use_empty=True)
print("importing USD:", usd, flush=True)
bpy.ops.wm.usd_import(filepath=usd)

meshes = [o for o in bpy.data.objects if o.type == 'MESH' and o.data.vertices]
def ctr(o):
    bb=[o.matrix_world @ Vector(c) for c in o.bound_box]; return sum(bb,Vector((0,0,0)))/8.0
def wdim(o):
    bb=[o.matrix_world @ Vector(c) for c in o.bound_box]
    xs=[v.x for v in bb]; ys=[v.y for v in bb]; zs=[v.z for v in bb]
    return max(max(xs)-min(xs),max(ys)-min(ys),max(zs)-min(zs))
# drop sky-dome giant outliers
ms=sorted(meshes,key=wdim,reverse=True)
while len(ms)>2 and wdim(ms[0])>3*wdim(ms[1]) and wdim(ms[0])>300:
    bpy.data.objects.remove(ms[0],do_unlink=True); ms=ms[1:]
meshes=[o for o in bpy.data.objects if o.type=='MESH' and o.data.vertices]
# central subset
def med(v): s=sorted(v); return s[len(s)//2]
cs=[ctr(o) for o in meshes]
C=Vector((med([c.x for c in cs]),med([c.y for c in cs]),med([c.z for c in cs])))
keep=[o for o in meshes if (ctr(o)-C).length<=R]
for o in [o for o in meshes if o not in keep]: bpy.data.objects.remove(o,do_unlink=True)
print("kept",len(keep),"meshes within R",R,flush=True)

# --- average color per material (from its base-color texture) ---
def avg_color(mat):
    if not mat or not mat.use_nodes: return (0.6,0.6,0.6)
    # find an image texture node
    img=None
    for n in mat.node_tree.nodes:
        if n.type=='TEX_IMAGE' and n.image is not None:
            img=n.image; break
    if img is None:
        # try principled base color default
        for n in mat.node_tree.nodes:
            if n.type=='BSDF_PRINCIPLED':
                c=n.inputs['Base Color'].default_value; return (c[0],c[1],c[2])
        return (0.6,0.6,0.6)
    try:
        w,h=img.size
        px=np.array(img.pixels[:], dtype=np.float32)
        if px.size < 4: return (0.6,0.6,0.6)
        px=px.reshape(-1,4)
        # sample up to 10000 pixels for speed
        if px.shape[0] > 10000:
            idx=np.linspace(0,px.shape[0]-1,10000).astype(int); px=px[idx]
        # ignore fully transparent
        a=px[:,3]; m=a>0.1
        if m.sum()>0: px=px[m]
        mean=px[:,:3].mean(axis=0)
        return (float(mean[0]),float(mean[1]),float(mean[2]))
    except Exception as e:
        print("avg fail",mat.name,e,flush=True); return (0.6,0.6,0.6)

# join all
bpy.ops.object.select_all(action='DESELECT')
for o in keep: o.select_set(True)
bpy.context.view_layer.objects.active=keep[0]
bpy.ops.object.join()
obj=bpy.context.view_layer.objects.active
me=obj.data
print("joined",len(me.vertices),"verts,",len(me.materials),"mats",flush=True)

# precompute avg color per material slot
slot_col=[avg_color(m) for m in me.materials]
print("computed",len(slot_col),"slot colors",flush=True)

# create vertex color layer (per-loop)
if me.vertex_colors:
    vc=me.vertex_colors[0]
else:
    vc=me.vertex_colors.new(name="Col")
cols=slot_col if slot_col else [(0.6,0.6,0.6)]
# assign per polygon's material to its loops
for poly in me.polygons:
    mi=poly.material_index if poly.material_index < len(cols) else 0
    r,g,b=cols[mi]
    for li in poly.loop_indices:
        vc.data[li].color=(r,g,b,1.0)
print("vertex colors assigned",flush=True)

# reduce to ONE material (emission-ish using vertex color for Blender preview; UE uses the vertex colors on import)
for _ in range(len(me.materials)):
    obj.data.materials.pop(index=0)
m=bpy.data.materials.new("M_GalataVC"); m.use_nodes=True
obj.data.materials.append(m)
for poly in me.polygons: poly.material_index=0

# decimate to ~1M verts so UE imports without hanging (Collapse keeps vertex colors)
target=950000
if len(me.vertices) > target:
    ratio=min(1.0, target/len(me.vertices))
    dm=obj.modifiers.new("d",'DECIMATE'); dm.ratio=ratio
    bpy.context.view_layer.objects.active=obj
    bpy.ops.object.modifier_apply(modifier=dm.name)
    print(f"after decimate ratio={ratio:.3f}: {len(obj.data.vertices)} verts",flush=True)
me=obj.data

os.makedirs(os.path.dirname(fbx),exist_ok=True)
try:
    bpy.ops.export_scene.fbx(filepath=fbx, path_mode='COPY', embed_textures=False,
        bake_space_transform=False, object_types={'MESH'}, colors_type='SRGB')
except TypeError:
    bpy.ops.export_scene.fbx(filepath=fbx, path_mode='COPY', embed_textures=False,
        bake_space_transform=False, object_types={'MESH'})
sz=os.path.getsize(fbx) if os.path.exists(fbx) else 0
print(f"exported {fbx} size={sz/1e6:.1f}MB verts={len(me.vertices)} mats=1 (vertex-colored)",flush=True)