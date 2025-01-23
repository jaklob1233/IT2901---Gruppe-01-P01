"""Resource Profiler that unconvers resource constraints in the application"""
import cProfile
import logging

class ApplicationProfiler:
    """Resource Profiler that uncovers resource constraints in the application"""
    def __init__(self, application, file):
        self.profiler = cProfile.Profile()
        self.__check_validity(application, file)
        self.application = application
        self.file = file

    def __check_validity(self, application, file):
        if application is None:
            raise ValueError("Application is required")
        if file is None:
            raise ValueError("File is required")
        if not isinstance(file, str):
            raise ValueError("File must be a string")
        if not file.endswith(".prof"):
            raise ValueError("File must be a .prof file")

    def init_profiler(self):
        """Initialize the profiler"""
        self.profiler.enable()
        self.application.run()

    def dump_stats(self):
        """Dump the profiler statistics to the file"""
        self.profiler.disable()
        self.profiler.dump_stats(self.file)

    def run(self):
        """Run the application with the profiler enabled"""
        try:
            self.init_profiler()
        except KeyboardInterrupt:
            pass
        finally:
            self.dump_stats()
            logging.info("Profiling results written to %s", self.file)
            logging.info("Run 'python -m snakeviz %s' to view the results in a browser", self.file)
