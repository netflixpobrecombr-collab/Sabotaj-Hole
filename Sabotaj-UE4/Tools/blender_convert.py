import bpy, sys, os

argv = sys.argv[sys.argv.index("--") + 1:]
src_root, dst_root = argv[0], argv[1]

glbs = []
for dp, _, files in os.walk(src_root):
    for fn in files:
        if fn.lower().endswith(".glb"):
            glbs.append(os.path.join(dp, fn))
print(f"found {len(glbs)} glb", flush=True)

done = 0
for i, src in enumerate(glbs):
    rel = os.path.relpath(src, src_root)
    dst = os.path.join(dst_root, os.path.splitext(rel)[0] + ".fbx")
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    if os.path.exists(dst):
        done += 1
        continue
    try:
        bpy.ops.wm.read_factory_settings(use_empty=True)
        bpy.ops.import_scene.gltf(filepath=src)
        bpy.ops.export_scene.fbx(
            filepath=dst, path_mode='COPY', embed_textures=True,
            add_leaf_bones=False, bake_anim=False, mesh_smooth_type='FACE')
        done += 1
    except Exception as e:
        print(f"  !fail {rel}: {e}", flush=True)
    if (i + 1) % 25 == 0:
        print(f"...{i+1}/{len(glbs)}  ({done} ok)", flush=True)

print(f"DONE converted {done}/{len(glbs)} to FBX", flush=True)