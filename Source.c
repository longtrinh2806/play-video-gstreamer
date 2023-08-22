#include<gst/gst.h>

int main(int argc, char* argv[]) {
	GstElement* pipeline, * source, * sink, * decoder, * filter;
	GstBus* bus;
	GstMessage* message;
	GstStateChangeReturn ret;

	gst_init(&argc, &argv);

	// Empty pipeline
	pipeline = gst_pipeline_new("my-pipeline");
	source = gst_element_factory_make("filesrc", "file-source");
	decoder = gst_element_factory_make("decodebin", "decode-bin");
	filter = gst_element_factory_make("videoconvert", "video-convert");
	sink = gst_element_factory_make("autovideosink", "auto-video-sink");

	g_object_set(G_OBJECT(source), "location", "file:///C:/Users/trinh/Desktop/test-video.mp4", NULL);

	if (!pipeline || !source || !decoder || !filter || !sink) {
		g_printerr("Can not create all elements.\n");
		return -1;
	}

	gst_bin_add_many(GST_BIN(pipeline), source, decoder, filter, sink, NULL);

	if (gst_element_link(source, decoder, filter, sink) != TRUE) {
		g_printerr("Can not link the elements.\n");
		gst_object_unref(pipeline);
		return -1;
	}


	// Start playing
	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);

	if (ret != GST_STATE_CHANGE_SUCCESS) {
		g_printerr("Failed to set the pipeline to PLAYING: %s\n", gst_element_state_change_return_get_name(ret));
		gst_object_unref(pipeline);
		return -1;
	}

	bus = gst_element_get_bus(pipeline);
	message = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
		GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
	if (message != NULL) {
		GError* err;
		gchar* debug_info;
		switch (GST_MESSAGE_TYPE(message)) {
		case GST_MESSAGE_ERROR:
			gst_message_parse_error(message, &err, &debug_info);
			g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(message->src),
				err->message);
			g_printerr("Debugging Info: %s\n", debug_info ? debug_info : "none");
			g_clear_error(&err);
			g_free(debug_info);
			break;
		case GST_MESSAGE_EOS:
			g_print("The End.");
		default:
			g_printerr("Unexpected message received.\n");
			break;
		}
		gst_message_unref(message);
	}
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	return 0;
}