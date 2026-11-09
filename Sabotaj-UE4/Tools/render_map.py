import bpy, sys, os
from mathutils import Vector
argv = sys.argv[sys.argv.index("--") + 1:]
usd, outpng = argv[0], argv[1]

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.wm.usd_import(filepath=usd)

# Remove sky/atmosphere/huge backdrop objects so they don't dominate framing
BAD = ("sky", "sphere", "cloud", "atmos", "fog", "backdrop", "dome", "star")
for o in list(bpy.data.objects):
    nm = o.name.lower()
    if any(b in nm for b in BAD):
        bpy.data.objects.remove(o, do_unlink=True)

objs = [o for o in bpy.data.objects if o.type == 'MESH' and o.data.vertices]
def dim(o):
    bb = [o.matrix_world @ Vector(c) for c in o.bound_box]
    xs = [v.x for v in bb]; ys = [v.y for v in bb]; zs = [v.z for v in bb]
    return max(max(xs)-min(xs), max(ys)-min(ys), max(zs)-min(zs))
pairs = sorted(((dim(o), o) for o in objs), key=lambda p: p[0], reverse=True)
print("TOP OBJECTS:", flush=True)
for d, o in pairs[:12]:
    print(f"   {round(d):>8}  {o.name}", flush=True)
# DELETE clear giant outliers (sky dome etc.): > 4x the second-largest object
second = pairs[1][0] if len(pairs) > 1 else (pairs[0][0] if pairs else 1e18)
cap = second * 4
for d, o in pairs:
    if d > cap:
        print("deleting giant:", o.name, round(d), flush=True)
        bpy.data.objects.remove(o, do_unlink=True)
keep = [o for d, o in pairs if d <= cap]
mins = Vector((1e18, 1e18, 1e18)); maxs = Vector((-1e18, -1e18, -1e18))
for o in keep:
    for c in o.bound_box:
        w = o.matrix_world @ Vector(c)
        for i in range(3):
            mins[i] = min(mins[i], w[i]); maxs[i] = max(maxs[i], w[i])
center = (mins + maxs) / 2
sx = maxs.x - mins.x; sy = maxs.y - mins.y; sz = maxs.z - mins.z
diag = (maxs - mins).length
print("kept:", len(keep), "size x/y/z:", round(sx), round(sy), round(sz), "center:", [round(x) for x in center], flush=True)

# Top-down orthographic camera (map plan view)
cam_data = bpy.data.cameras.new("Cam"); cam = bpy.data.objects.new("Cam", cam_data)
bpy.context.scene.collection.objects.link(cam)
cam_data.type = 'ORTHO'
cam_data.ortho_scale = max(sx, sy) * 1.08
cam_data.clip_start = 1.0
cam_data.clip_end = (sz + max(sx, sy)) * 4
cam.location = Vector((center.x, center.y, maxs.z + max(sx, sy)))
cam.rotation_euler = (0.0, 0.0, 0.0)  # looking straight down -Z
bpy.context.scene.camera = cam

scn = bpy.context.scene
scn.render.engine = 'BLENDER_WORKBENCH'
scn.render.resolution_x = 1600; scn.render.resolution_y = 900
scn.render.filepath = outpng
try:
    scn.display.shading.light = 'STUDIO'
    scn.display.shading.color_type = 'TEXTURE'
    scn.display.shading.show_cavity = True
    scn.display.shading.show_shadows = True
except Exception as e:
    print("shading cfg:", e, flush=True)
os.makedirs(os.path.dirname(outpng), exist_ok=True)
bpy.ops.render.render(write_still=True)
print("rendered", outpng, os.path.getsize(outpng) if os.path.exists(outpng) else 0, flush=True)