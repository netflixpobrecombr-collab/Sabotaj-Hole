import bpy, sys, os
from mathutils import Vector
import numpy as np
argv = sys.argv[sys.argv.index("--") + 1:]
usd, fbx = argv[0], argv[1]
R = float(argv[2]) if len(argv) > 2 else 48.0
TARGET = int(argv[3]) if len(argv) > 3 else 950000
RES = int(argv[4]) if len(argv) > 4 else 2048

bpy.ops.wm.read_factory_settings(use_empty=True)
print("importing USD:", usd, flush=True)
bpy.ops.wm.usd_import(filepath=usd)

meshes=[o for o in bpy.data.objects if o.type=='MESH' and o.data.vertices]
def ctr(o):
    bb=[o.matrix_world@Vector(c) for c in o.bound_box]; return sum(bb,Vector((0,0,0)))/8.0
def wdim(o):
    bb=[o.matrix_world@Vector(c) for c in o.bound_box]
    xs=[v.x for v in bb];ys=[v.y for v in bb];zs=[v.z for v in bb]
    return max(max(xs)-min(xs),max(ys)-min(ys),max(zs)-min(zs))
ms=sorted(meshes,key=wdim,reverse=True)
while len(ms)>2 and wdim(ms[0])>3*wdim(ms[1]) and wdim(ms[0])>300:
    bpy.data.objects.remove(ms[0],do_unlink=True); ms=ms[1:]
meshes=[o for o in bpy.data.objects if o.type=='MESH' and o.data.vertices]
def med(v):s=sorted(v);return s[len(s)//2]
cs=[ctr(o) for o in meshes]
C=Vector((med([c.x for c in cs]),med([c.y for c in cs]),med([c.z for c in cs])))
keep=[o for o in meshes if (ctr(o)-C).length<=R]
for o in [o for o in meshes if o not in keep]: bpy.data.objects.remove(o,do_unlink=True)
print("kept",len(keep),"meshes",flush=True)

def avg_color(mat):
    if not mat or not mat.use_nodes: return (0.6,0.6,0.6)
    img=None
    for n in mat.node_tree.nodes:
        if n.type=='TEX_IMAGE' and n.image is not None: img=n.image; break
    if img is None:
        for n in mat.node_tree.nodes:
            if n.type=='BSDF_PRINCIPLED':
                c=n.inputs['Base Color'].default_value; return (c[0],c[1],c[2])
        return (0.6,0.6,0.6)
    try:
        px=np.array(img.pixels[:],dtype=np.float32)
        if px.size<4: return (0.6,0.6,0.6)
        px=px.reshape(-1,4)
        if px.shape[0]>8000:
            idx=np.linspace(0,px.shape[0]-1,8000).astype(int);px=px[idx]
        a=px[:,3];m=a>0.1
        if m.sum()>0: px=px[m]
        mean=px[:,:3].mean(axis=0)
        return (float(mean[0]),float(mean[1]),float(mean[2]))
    except Exception: return (0.6,0.6,0.6)

bpy.ops.object.select_all(action='DESELECT')
for o in keep:o.select_set(True)
bpy.context.view_layer.objects.active=keep[0]
bpy.ops.object.join()
obj=bpy.context.view_layer.objects.active; me=obj.data
print("joined",len(me.vertices),"verts,",len(me.materials),"mats",flush=True)

# decimate FIRST (fewer verts to UV/bake)
if len(me.vertices)>TARGET:
    r=min(1.0,TARGET/len(me.vertices))
    dm=obj.modifiers.new("d",'DECIMATE');dm.ratio=r
    bpy.ops.object.modifier_apply(modifier=dm.name)
    me=obj.data
    print("decimated to",len(me.vertices),"verts",flush=True)

# per-slot avg colors -> vertex colors
slot_col=[avg_color(m) for m in me.materials] or [(0.6,0.6,0.6)]
vc=me.vertex_colors.new(name="Col")
for poly in me.polygons:
    mi=poly.material_index if poly.material_index<len(slot_col) else 0
    r,g,b=slot_col[mi]
    for li in poly.loop_indices: vc.data[li].color=(r,g,b,1.0)
print("vertex colors set",flush=True)

# smart UV unwrap
bpy.ops.object.mode_set(mode='EDIT')
bpy.ops.mesh.select_all(action='SELECT')
bpy.ops.uv.smart_project(angle_limit=66.0, island_margin=0.002)
bpy.ops.object.mode_set(mode='OBJECT')
print("uv unwrapped",flush=True)

# one material: emission = vertex color, with target image for bake
for _ in range(len(me.materials)): obj.data.materials.pop(index=0)
mat=bpy.data.materials.new("M_GalataBaked"); mat.use_nodes=True
obj.data.materials.append(mat)
for poly in me.polygons: poly.material_index=0
nt=mat.node_tree; nt.nodes.clear()
out=nt.nodes.new("ShaderNodeOutputMaterial")
emit=nt.nodes.new("ShaderNodeEmission")
vcn=nt.nodes.new("ShaderNodeVertexColor"); vcn.layer_name="Col"
nt.links.new(vcn.outputs['Color'], emit.inputs['Color'])
nt.links.new(emit.outputs['Emission'], out.inputs['Surface'])
img=bpy.data.images.new("GalataBake", RES, RES)
texn=nt.nodes.new("ShaderNodeTexImage"); texn.image=img
nt.nodes.active=texn

# bake EMIT (flat, captures vertex colors)
scn=bpy.context.scene
scn.render.engine='CYCLES'
try: scn.cycles.device='CPU'
except Exception: pass
scn.cycles.samples=1
scn.render.bake.use_pass_direct=False
scn.render.bake.use_pass_indirect=False
scn.render.bake.margin=4
print("baking...",flush=True)
bpy.ops.object.bake(type='EMIT')
print("baked",flush=True)

# save texture next to fbx
texdir=os.path.dirname(fbx); os.makedirs(texdir,exist_ok=True)
pngp=os.path.join(texdir,"GalataBake.png")
img.filepath_raw=pngp; img.file_format='PNG'; img.save()
print("saved texture",pngp,flush=True)

# rebuild material to use baked texture as base color (standard, UE-friendly)
nt.nodes.clear()
out2=nt.nodes.new("ShaderNodeOutputMaterial")
bsdf=nt.nodes.new("ShaderNodeBsdfPrincipled")
tex2=nt.nodes.new("ShaderNodeTexImage"); tex2.image=img
nt.links.new(tex2.outputs['Color'], bsdf.inputs['Base Color'])
nt.links.new(bsdf.outputs['BSDF'], out2.inputs['Surface'])

bpy.ops.export_scene.fbx(filepath=fbx, path_mode='COPY', embed_textures=False,
    bake_space_transform=False, object_types={'MESH'})
sz=os.path.getsize(fbx) if os.path.exists(fbx) else 0
print(f"exported {fbx} size={sz/1e6:.1f}MB verts={len(me.vertices)} (baked texture)",flush=True)