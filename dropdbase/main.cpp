#include <cstdio>
#include <spdlog_setup/conf.h>

int main(int argc, char** argv)
{
	try {
		spdlog_setup::from_file("../configuration/log_config.toml"); // configure logger
		auto log_ = spdlog::get("root"); // get logger
		log_->info("Application started.");

		// TODO main loop

		log_->info("Application exited.");
	}
	catch (const spdlog_setup::setup_error &exception) { // log config file not found
		fprintf(stderr, "Cannot setup logger: %s\n", exception.what());
	}
	catch (const std::exception &exception) {
		fprintf(stderr, "Exception: %s\n", exception.what());
	}
	return 0;
}
