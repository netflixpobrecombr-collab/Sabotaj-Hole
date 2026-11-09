import bpy, sys, os, math
from mathutils import Vector
argv = sys.argv[sys.argv.index("--") + 1:]
src, outpng = argv[0], argv[1]
azim_deg = float(argv[2]) if len(argv) > 2 else 55.0
elev_deg = float(argv[3]) if len(argv) > 3 else 25.0
street = (len(argv) > 4 and argv[4] == "street")
full_scene = (len(argv) > 4 and argv[4] == "full")

bpy.ops.wm.read_factory_settings(use_empty=True)
print("importing:", src, flush=True)
if src.lower().endswith(".fbx"):
    bpy.ops.import_scene.fbx(filepath=src)
else:
    bpy.ops.wm.usd_import(filepath=src)

# --- remove sky/dome/atmosphere by name, and mega-objects by size ---
BAD = ("sky","dome","skydome","skysphere","atmos","cloud","backdrop","fog","star","horizon","ocean","water_plane","killz","blockingvolume")
def wdim(o):
    bb=[o.matrix_world @ Vector(c) for c in o.bound_box]
    xs=[v.x for v in bb]; ys=[v.y for v in bb]; zs=[v.z for v in bb]
    return max(max(xs)-min(xs), max(ys)-min(ys), max(zs)-min(zs))
if not full_scene:
    # remove by name first
    for o in list(bpy.data.objects):
        if o.type!='MESH': continue
        if any(b in o.name.lower() for b in BAD):
            print("remove(name):", o.name, flush=True)
            try: bpy.data.objects.remove(o, do_unlink=True)
            except Exception: pass
    # iteratively peel off giant outliers (sky domes / reflection spheres): largest > 3x next-largest
    for _ in range(10):
        ms = sorted((o for o in bpy.data.objects if o.type=='MESH' and o.data.vertices), key=wdim, reverse=True)
        if len(ms) < 3: break
        d0, d1 = wdim(ms[0]), wdim(ms[1])
        if d0 > 3*d1 and d0 > 300:
            print("remove(outlier):", ms[0].name, round(d0), "vs next", round(d1), flush=True)
            try: bpy.data.objects.remove(ms[0], do_unlink=True)
            except Exception: pass
        else:
            break
print("TOP sizes:", [(o.name, round(wdim(o))) for o in sorted((o for o in bpy.data.objects if o.type=='MESH' and o.data.vertices), key=wdim, reverse=True)[:6]], flush=True)

objs = [o for o in bpy.data.objects if o.type == 'MESH' and o.data.vertices]
def obj_center(o):
    bb = [o.matrix_world @ Vector(c) for c in o.bound_box]
    return sum(bb, Vector((0,0,0)))/8.0
def median(vals):
    s = sorted(vals); n = len(s)
    return s[n//2] if n % 2 else (s[n//2-1]+s[n//2])/2
if full_scene:
    # Keep every mesh imported, but hide presentation-only surfaces in the
    # preview. Their material tags are semantic (not a name or size heuristic).
    candidates = []
    for o in objs:
        material_names = [mat.name.lower() for mat in o.data.materials if mat]
        if any(name.startswith("m_sky") for name in material_names):
            o.hide_render = True
            print("hide sky shell:", o.name, flush=True)
        elif any(name.startswith(("m_pureblack", "m_water_opaque")) for name in material_names):
            o.hide_render = True
            print("hide presentation material:", o.name, flush=True)
        else:
            candidates.append(o)
    centers = {o: obj_center(o) for o in candidates}
    med = Vector((median([c.x for c in centers.values()]),
                  median([c.y for c in centers.values()]),
                  median([c.z for c in centers.values()])))
    dists = sorted((centers[o]-med).length for o in candidates)
    # One recovered fence is 9 km from the level, while 99% of the level is
    # within 64 units. Use a generous data-derived radius to keep the scene.
    frame_limit = max(300.0, dists[int(0.99*(len(dists)-1))] * 5.0)
    keep = []
    for o in candidates:
        if (centers[o]-med).length > frame_limit:
            o.hide_render = True
            print("exclude distant object:", o.name, flush=True)
        else:
            keep.append(o)
    print("full scene", "keep", len(keep), "/", len(objs), "frame limit", round(frame_limit), flush=True)
else:
    centers = {o: obj_center(o) for o in objs}
    med = Vector((median([c.x for c in centers.values()]),
                  median([c.y for c in centers.values()]),
                  median([c.z for c in centers.values()])))
    dists = sorted((centers[o]-med).length for o in objs)
    # keep objects within the 70th percentile distance of the median -> the main building cluster
    thresh = dists[int(0.70*(len(dists)-1))] * 1.4 + 1.0
    keep = [o for o in objs if (centers[o]-med).length <= thresh]
    print("median", [round(x) for x in med], "thresh", round(thresh), "keep", len(keep), "/", len(objs), flush=True)

mins = Vector((1e18,)*3); maxs = Vector((-1e18,)*3)
for o in keep:
    for c in o.bound_box:
        w = o.matrix_world @ Vector(c)
        for i in range(3):
            mins[i]=min(mins[i],w[i]); maxs[i]=max(maxs[i],w[i])
center = (mins + maxs)/2
radius = (maxs - mins).length / 2
print("cluster center", [round(x) for x in center], "radius", round(radius), flush=True)

# --- Camera, fitted to bbox sphere ---
cam_data = bpy.data.cameras.new("Cam"); cam = bpy.data.objects.new("Cam", cam_data)
bpy.context.scene.collection.objects.link(cam)
cam_data.lens = 40
scn = bpy.context.scene
scn.render.resolution_x = 1920; scn.render.resolution_y = 1080
# vertical fov from lens + sensor; fit sphere into min(h,w) fov
import math as _m
sensor = cam_data.sensor_width
vfov = 2*_m.atan((sensor*scn.render.resolution_y/scn.render.resolution_x)/(2*cam_data.lens))
hfov = 2*_m.atan(sensor/(2*cam_data.lens))
fit = min(vfov, hfov)
elev = _m.radians(elev_deg); azim = _m.radians(azim_deg)
if street:
    cam_data.lens = 24  # wide, immersive
    h = maxs.z - mins.z
    eye = mins.z + h*0.08
    back = radius * 0.55
    cam.location = Vector((center.x - _m.cos(azim)*back, center.y - _m.sin(azim)*back, eye))
    look = Vector((center.x, center.y, mins.z + h*0.22))
    cam.rotation_euler = (look - cam.location).to_track_quat('-Z','Y').to_euler()
    cam_data.clip_start = 0.1; cam_data.clip_end = radius*8
else:
    dist = radius / _m.sin(fit/2) * 1.15
    offset = Vector((_m.cos(elev)*_m.cos(azim), _m.cos(elev)*_m.sin(azim), _m.sin(elev)))*dist
    cam.location = center + offset
    cam.rotation_euler = (center - cam.location).to_track_quat('-Z','Y').to_euler()
    cam_data.clip_start = 1.0; cam_data.clip_end = dist*8
scn.camera = cam

# --- Workbench shading (predictable, shows textures) ---
scn.render.engine = 'BLENDER_WORKBENCH'
world = bpy.data.worlds.new("W"); scn.world = world
try: world.color = (0.6, 0.68, 0.78)
except Exception: pass
sh = scn.display.shading
sh.light = 'STUDIO'
sh.color_type = 'TEXTURE'
sh.show_cavity = True
sh.cavity_type = 'WORLD'
sh.show_shadows = True
sh.shadow_intensity = 0.5
try: sh.studio_light = 'outdoor.sl'
except Exception as e: print("studio:", e, flush=True)
try:
    scn.view_settings.view_transform = 'Standard'
except Exception: pass
scn.render.film_transparent = False
scn.render.filepath = outpng

os.makedirs(os.path.dirname(outpng), exist_ok=True)
bpy.ops.render.render(write_still=True)
print("RENDERED", outpng, os.path.getsize(outpng) if os.path.exists(outpng) else 0, flush=True)