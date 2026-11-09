import bpy, sys, os
argv = sys.argv[sys.argv.index("--") + 1:]
usd, fbx = argv[0], argv[1]

bpy.ops.wm.read_factory_settings(use_empty=True)
print("importing USD:", usd, flush=True)
try:
    bpy.ops.wm.usd_import(filepath=usd)
except Exception as e:
    print("usd_import error:", e, flush=True)
    raise

meshes = [o for o in bpy.data.objects if o.type == 'MESH']
verts = sum(len(o.data.vertices) for o in meshes)
print(f"imported objects total={len(bpy.data.objects)} meshes={len(meshes)} verts={verts}", flush=True)

os.makedirs(os.path.dirname(fbx), exist_ok=True)
bpy.ops.export_scene.fbx(filepath=fbx, path_mode='COPY', embed_textures=False,
                         bake_space_transform=False, object_types={'MESH', 'EMPTY'})
sz = os.path.getsize(fbx) if os.path.exists(fbx) else 0
print(f"exported FBX {fbx} size={sz/1e6:.1f}MB", flush=True)