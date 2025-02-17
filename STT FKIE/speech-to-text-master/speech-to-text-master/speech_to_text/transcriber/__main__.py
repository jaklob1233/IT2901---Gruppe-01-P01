#!/usr/bin/env python3

import base64

from configobj import ConfigObj

from speech_to_text.transcriber.transcriber import Transcriber


def main():
    filename = "base64_example.txt"
    sample_rate = 16000
    length = 32000 #Bytes

    f = open(filename, "r")
    str = f.read()
    f.close()
    bytes = base64.b64decode(str)

    transcriber = Transcriber("Whisper", ConfigObj("../config.ini"))
    transcriber.accept_data(bytes)
    print(transcriber.get_results())

if __name__ == '__main__':
    main()
