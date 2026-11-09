import bpy, sys, os
argv = sys.argv[sys.argv.index("--") + 1:]
usd, fbx, ratio = argv[0], argv[1], float(argv[2]) if len(argv) > 2 else 0.2

bpy.ops.wm.read_factory_settings(use_empty=True)
print("importing USD:", usd, flush=True)
bpy.ops.wm.usd_import(filepath=usd)

meshes = [o for o in bpy.data.objects if o.type == 'MESH' and o.data.vertices]
v0 = sum(len(o.data.vertices) for o in meshes)
print(f"before: {len(meshes)} meshes, {v0} verts", flush=True)

# decimate each mesh
for i, o in enumerate(meshes):
    try:
        if len(o.data.polygons) < 50:
            continue  # tiny meshes: leave as-is
        m = o.modifiers.new("dec", 'DECIMATE')
        m.ratio = ratio
        # apply
        bpy.context.view_layer.objects.active = o
        bpy.ops.object.modifier_apply(modifier=m.name)
    except Exception as e:
        print("  dec fail", o.name, e, flush=True)
    if i % 100 == 0:
        print(f"  decimated {i}/{len(meshes)}", flush=True)

v1 = sum(len(o.data.vertices) for o in meshes)
print(f"after: {v1} verts (from {v0})", flush=True)

os.makedirs(os.path.dirname(fbx), exist_ok=True)
bpy.ops.export_scene.fbx(filepath=fbx, path_mode='COPY', embed_textures=False,
                         bake_space_transform=False, object_types={'MESH', 'EMPTY'})
sz = os.path.getsize(fbx) if os.path.exists(fbx) else 0
print(f"exported FBX {fbx} size={sz/1e6:.1f}MB verts={v1}", flush=True)