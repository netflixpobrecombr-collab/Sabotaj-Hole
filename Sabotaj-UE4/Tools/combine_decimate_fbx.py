import bpy, sys, os
argv = sys.argv[sys.argv.index("--") + 1:]
usd, fbx, target = argv[0], argv[1], int(argv[2]) if len(argv) > 2 else 500000

bpy.ops.wm.read_factory_settings(use_empty=True)
print("importing USD:", usd, flush=True)
bpy.ops.wm.usd_import(filepath=usd)

meshes = [o for o in bpy.data.objects if o.type == 'MESH' and o.data.vertices]
v0 = sum(len(o.data.vertices) for o in meshes)
print(f"before: {len(meshes)} meshes, {v0} verts", flush=True)

# join all meshes into one
bpy.ops.object.select_all(action='DESELECT')
for o in meshes:
    o.select_set(True)
bpy.context.view_layer.objects.active = meshes[0]
bpy.ops.object.join()
joined = bpy.context.view_layer.objects.active
print(f"joined -> {len(joined.data.vertices)} verts, {len(joined.data.materials)} material slots", flush=True)

# decimate joined mesh to target vert count
ratio = min(1.0, target / max(1, len(joined.data.vertices)))
m = joined.modifiers.new("dec", 'DECIMATE')
m.ratio = ratio
bpy.ops.object.modifier_apply(modifier=m.name)
v1 = len(joined.data.vertices)
print(f"after decimate ratio={ratio:.3f}: {v1} verts", flush=True)

os.makedirs(os.path.dirname(fbx), exist_ok=True)
bpy.ops.export_scene.fbx(filepath=fbx, path_mode='COPY', embed_textures=False,
                         bake_space_transform=False, object_types={'MESH'})
sz = os.path.getsize(fbx) if os.path.exists(fbx) else 0
print(f"exported FBX {fbx} size={sz/1e6:.1f}MB verts={v1}", flush=True)