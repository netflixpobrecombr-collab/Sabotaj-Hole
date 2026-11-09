import bpy, sys, os
argv = sys.argv[sys.argv.index("--")+1:]
for f in argv:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    try:
        bpy.ops.import_scene.fbx(filepath=f)
        meshes = [o for o in bpy.data.objects if o.type == 'MESH']
        arms   = [o for o in bpy.data.objects if o.type == 'ARMATURE']
        verts  = sum(len(m.data.vertices) for m in meshes)
        imgs   = len(bpy.data.images)
        print(f"OK {os.path.basename(f)} | meshes={len(meshes)} armatures={len(arms)} verts={verts} images={imgs}", flush=True)
    except Exception as e:
        print(f"FAIL {os.path.basename(f)}: {e}", flush=True)