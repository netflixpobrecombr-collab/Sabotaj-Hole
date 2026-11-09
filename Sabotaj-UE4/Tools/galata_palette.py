import bpy, sys, os, math
from mathutils import Vector
import numpy as np
argv = sys.argv[sys.argv.index("--") + 1:]
usd, fbx = argv[0], argv[1]
R = float(argv[2]) if len(argv) > 2 else 48.0
TARGET = int(argv[3]) if len(argv) > 3 else 950000

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
# colors per slot BEFORE decimate (materials intact)
slot_col=[avg_color(m) for m in me.materials] or [(0.6,0.6,0.6)]
N=len(slot_col)
print("N materials",N,flush=True)

# decimate (keeps material_index)
if len(me.vertices)>TARGET:
    r=min(1.0,TARGET/len(me.vertices))
    dm=obj.modifiers.new("d",'DECIMATE');dm.ratio=r
    bpy.ops.object.modifier_apply(modifier=dm.name)
    me=obj.data
    print("decimated to",len(me.vertices),"verts",flush=True)

# --- build palette image: grid of cells, each cell = one material color ---
CELL=16
G=int(math.ceil(math.sqrt(N)))      # grid GxG
S=G*CELL                            # image size
pix=np.zeros((S,S,4),dtype=np.float32); pix[...,3]=1.0
for i in range(N):
    gx=i%G; gy=i//G
    r,g,b=slot_col[i]
    pix[gy*CELL:(gy+1)*CELL, gx*CELL:(gx+1)*CELL, 0]=r
    pix[gy*CELL:(gy+1)*CELL, gx*CELL:(gx+1)*CELL, 1]=g
    pix[gy*CELL:(gy+1)*CELL, gx*CELL:(gx+1)*CELL, 2]=b
img=bpy.data.images.new("GalataPal", S, S)
img.pixels=pix.reshape(-1).tolist()
texdir=os.path.dirname(fbx); os.makedirs(texdir,exist_ok=True)
pngp=os.path.join(texdir,"GalataPal.png")
img.filepath_raw=pngp; img.file_format='PNG'; img.save()
print(f"palette {S}x{S} saved, grid {G}x{G} cell {CELL}",flush=True)

# --- set every face's UVs to the center of its material's palette cell ---
uv=me.uv_layers.new(name="Pal")
for poly in me.polygons:
    mi=poly.material_index if poly.material_index<N else 0
    gx=mi%G; gy=mi//G
    u=(gx*CELL + CELL*0.5)/S
    v=(gy*CELL + CELL*0.5)/S
    for li in poly.loop_indices:
        uv.data[li].uv=(u, v)
me.uv_layers.active=uv
print("palette UVs assigned",flush=True)

# one material using the palette texture
for _ in range(len(me.materials)): obj.data.materials.pop(index=0)
mat=bpy.data.materials.new("M_GalataPal"); mat.use_nodes=True
obj.data.materials.append(mat)
for poly in me.polygons: poly.material_index=0
nt=mat.node_tree; nt.nodes.clear()
out=nt.nodes.new("ShaderNodeOutputMaterial")
bsdf=nt.nodes.new("ShaderNodeBsdfPrincipled")
tex=nt.nodes.new("ShaderNodeTexImage"); tex.image=img; tex.interpolation='Closest'
nt.links.new(tex.outputs['Color'], bsdf.inputs['Base Color'])
nt.links.new(bsdf.outputs['BSDF'], out.inputs['Surface'])

bpy.ops.export_scene.fbx(filepath=fbx, path_mode='COPY', embed_textures=False,
    bake_space_transform=False, object_types={'MESH'})
sz=os.path.getsize(fbx) if os.path.exists(fbx) else 0
print(f"exported {fbx} size={sz/1e6:.1f}MB verts={len(me.vertices)} (palette)",flush=True)