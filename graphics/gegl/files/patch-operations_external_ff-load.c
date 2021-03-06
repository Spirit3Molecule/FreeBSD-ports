https://git.gnome.org/browse/gegl/commit/?id=97067622352e
https://git.gnome.org/browse/gegl/commit/?id=6d50c42e2c9a
https://git.gnome.org/browse/gegl/commit/?id=67f14cbbc5d1

--- operations/external/ff-load.c.orig	2012-04-01 11:17:57 UTC
+++ operations/external/ff-load.c
@@ -137,7 +137,7 @@ ff_cleanup (GeglChantO *o)
       if (p->enc)
         avcodec_close (p->enc);
       if (p->ic)
-        av_close_input_file (p->ic);
+        avformat_close_input(&p->ic);
       if (p->lavc_frame)
         av_free (p->lavc_frame);
 
@@ -216,9 +216,9 @@ decode_frame (GeglOperation *operation,
             {
               do
                 {
-                  if (av_read_packet (p->ic, &p->pkt) < 0)
+                  if (av_read_frame (p->ic, &p->pkt) < 0)
                     {
-                      fprintf (stderr, "av_read_packet failed for %s\n",
+                      fprintf (stderr, "av_read_frame failed for %s\n",
                                o->path);
                       return -1;
                     }
@@ -271,12 +271,12 @@ prepare (GeglOperation *operation)
       gint err;
 
       ff_cleanup (o);
-      err = av_open_input_file (&p->ic, o->path, NULL, 0, NULL);
+      err = avformat_open_input(&p->ic, o->path, NULL, 0);
       if (err < 0)
         {
           print_error (o->path, err);
         }
-      err = av_find_stream_info (p->ic);
+      err = avformat_find_stream_info (p->ic, NULL);
       if (err < 0)
         {
           g_warning ("ff-load: error finding stream info for %s", o->path);
@@ -309,10 +309,10 @@ prepare (GeglOperation *operation)
           g_warning ("codec not found");
         }
 
-      if (p->codec->capabilities & CODEC_CAP_TRUNCATED)
-        p->enc->flags |= CODEC_FLAG_TRUNCATED;
+      if (p->codec->capabilities & AV_CODEC_CAP_TRUNCATED)
+        p->enc->flags |= AV_CODEC_FLAG_TRUNCATED;
 
-      if (avcodec_open (p->enc, p->codec) < 0)
+      if (avcodec_open2 (p->enc, p->codec, NULL) < 0)
         {
           g_warning ("error opening codec %s", p->enc->codec->name);
           return;
@@ -321,7 +321,7 @@ prepare (GeglOperation *operation)
       p->width = p->enc->width;
       p->height = p->enc->height;
       p->frames = 10000000;
-      p->lavc_frame = avcodec_alloc_frame ();
+      p->lavc_frame = av_frame_alloc ();
 
       if (p->fourcc)
         g_free (p->fourcc);
