"""Linphone core testing class""" 
import sys
import unittest
import socket
import pyaudio
sys.path.append('../')
#pylint: disable=import-error
from linphone_module import Core#pylint: disable=wrong-import-position

class TestLinphoneCoreFunctionality(unittest.TestCase):
    """Test class that tests the core functionality of python"""
    def test_create_linphone_core(self):
        """
        This test case is used to test the creation of the linphone core
        Makes sure the core is connected to the correct port
        """
        port = 5060
        core = Core()
        core.start()

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        result = sock.connect_ex(('localhost', port))

        if result == 0:
            print(f"Core connected to correct port {port}")
            sock.close()
        else:
            print(f"Port {port} is available")
            sock.close()
            self.fail("Core has not connected to {port}")


    def test_start_linphone_core(self):
        """
        This text case tests the upstart of the core,
        to make sure it is ready to iterate
        """
        core = Core()

        self.assertTrue(core.start())

    def test_start_linphone_core_twice(self):
        """
        This test asserts that the core cannot be started twice,
        as having two recording processes makes little sense.
        If you want to record multiple streams, you should create
        another instance of the core, which would require it to run
        on a different port
        """
        core = Core()

        self.assertTrue(core.start())
        self.assertFalse(core.start())

    def test_stop_linphone_core(self):
        """
        This test case tests the stopping of the core,
        to make sure it is ready to stop and close the connection.
        Also ensures the correct behavior of the core when stopping
        without starting first
        """
        core = Core()

        self.assertFalse(core.stop())
        core.start()
        self.assertTrue(core.stop())

    def test_multiple_core_stops_and_restarts(self):
        """
        This test case tests multiple restarts, and stops of the core
        """
        core = Core()

        self.assertTrue(core.start())
        self.assertFalse(core.start())
        self.assertTrue(core.stop())

        self.assertTrue(core.start())
        self.assertFalse(core.start())
        self.assertTrue(core.stop())
        self.assertFalse(core.stop())

    def test_available_sound_sources(self):
        """
        This test case tests whether the correct audio device is
        available for the core to use
        """
        p = pyaudio.PyAudio()
        success = 0

        for i in range(p.get_device_count()):
            d = p.get_device_info_by_index(i)
            if d["name"] == "pulse":
                success = 1
                break

        self.assertTrue(success == 1)


if __name__ == "__main__":
    unittest.main()
