import bpy, sys, os
from mathutils import Vector
argv = sys.argv[sys.argv.index("--") + 1:]
usd, fbx = argv[0], argv[1]
R = float(argv[2]) if len(argv) > 2 else 50.0
target = int(argv[3]) if len(argv) > 3 else 400000

bpy.ops.wm.read_factory_settings(use_empty=True)
print("importing USD:", usd, flush=True)
bpy.ops.wm.usd_import(filepath=usd)

meshes = [o for o in bpy.data.objects if o.type == 'MESH' and o.data.vertices]
def ctr(o):
    bb=[o.matrix_world @ Vector(c) for c in o.bound_box]
    return sum(bb, Vector((0,0,0)))/8.0
def wdim(o):
    bb=[o.matrix_world @ Vector(c) for c in o.bound_box]
    xs=[v.x for v in bb]; ys=[v.y for v in bb]; zs=[v.z for v in bb]
    return max(max(xs)-min(xs), max(ys)-min(ys), max(zs)-min(zs))
# drop giant outliers (sky dome)
ms = sorted(meshes, key=wdim, reverse=True)
while len(ms) > 2 and wdim(ms[0]) > 3*wdim(ms[1]) and wdim(ms[0]) > 300:
    print("drop giant", ms[0].name, round(wdim(ms[0])), flush=True)
    bpy.data.objects.remove(ms[0], do_unlink=True); ms = ms[1:]
meshes = [o for o in bpy.data.objects if o.type=='MESH' and o.data.vertices]
# median center
def med(v): s=sorted(v); n=len(s); return s[n//2]
cs=[ctr(o) for o in meshes]
C=Vector((med([c.x for c in cs]), med([c.y for c in cs]), med([c.z for c in cs])))
print("center", [round(x) for x in C], "R", R, flush=True)
# keep meshes within R of center
keep=[o for o in meshes if (ctr(o)-C).length <= R]
drop=[o for o in meshes if o not in keep]
for o in drop:
    bpy.data.objects.remove(o, do_unlink=True)
v0=sum(len(o.data.vertices) for o in keep)
print(f"kept {len(keep)}/{len(meshes)} meshes within R, {v0} verts", flush=True)

# join
bpy.ops.object.select_all(action='DESELECT')
for o in keep: o.select_set(True)
bpy.context.view_layer.objects.active = keep[0]
bpy.ops.object.join()
j=bpy.context.view_layer.objects.active
print("joined", len(j.data.vertices), "verts,", len(j.data.materials), "mats", flush=True)
# decimate to target
ratio=min(1.0, target/max(1,len(j.data.vertices)))
m=j.modifiers.new("d",'DECIMATE'); m.ratio=ratio
bpy.ops.object.modifier_apply(modifier=m.name)
print(f"after decimate ratio={ratio:.3f}: {len(j.data.vertices)} verts", flush=True)

os.makedirs(os.path.dirname(fbx), exist_ok=True)
bpy.ops.export_scene.fbx(filepath=fbx, path_mode='COPY', embed_textures=False,
                         bake_space_transform=False, object_types={'MESH'})
sz=os.path.getsize(fbx) if os.path.exists(fbx) else 0
print(f"exported {fbx} size={sz/1e6:.1f}MB verts={len(j.data.vertices)}", flush=True)