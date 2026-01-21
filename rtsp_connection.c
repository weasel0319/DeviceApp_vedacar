#include <stdio.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

void *rtsp_connection(void *) {
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;

    // GStreamer 초기화 (스레드 안에서 수행)
    gst_init(NULL, NULL);

    loop = g_main_loop_new(NULL, FALSE);
    server = gst_rtsp_server_new();

    // 포트 설정 (기본 8554)
    gst_rtsp_server_set_service(server, "8554");

    mounts = gst_rtsp_server_get_mount_points(server);
    factory = gst_rtsp_media_factory_new();

    // 라즈베리 파이 카메라 파이프라인 설정
    // v4l2src: 카메라 입력 / v4l2h264enc: HW 가속 인코딩 / rtph264pay: RTP 패킷화
    gst_rtsp_media_factory_set_launch(factory,
    "( v4l2src device=/dev/video0 ! videoconvert ! "
    "video/x-raw,format=I420,width=320,height=180,framerate=30/1 ! " // 핵심: format=I420 추가
    "x264enc tune=zerolatency speed-preset=ultrafast bitrate=300 ! "
    "h264parse ! rtph264pay name=pay0 pt=96 )");

    // "rtsp://라즈베리IP:8554/live" 경로로 마운트
    gst_rtsp_mount_points_add_factory(mounts, "/live", factory);
    g_object_unref(mounts);

    // 서버 시작
    gst_rtsp_server_attach(server, NULL);

    printf("[RTSP] Server started at rtsp://0.0.0.0:8554/live\n");

    // 루프 실행 (여기서 스레드가 대기하며 서버 역할을 수행함)
    g_main_loop_run(loop);

    return NULL;
} 