class TextAudioSnippet:
    __text : str
    __audio : str
    __length : int
    # __timestamp : int
    __sample_rate : int

    def __init__(self, text="", audio="", length=0, sample_rate=16000):
        self.__text = text
        self.__audio = audio
        self.__length = length
        # self.__timestamp = timestamp
        self.__sample_rate = sample_rate

    def __repr__(self):
        return ("{"
                + "\n\ttext: " + repr(self.__text)
                + "\n\taudio: " + repr(self.__audio)
                + "\n\tlength: " + repr(self.__length)
                # + "\n\ttimestamp: " + repr(self.__timestamp)
                + "\n\tsample_rate: " + repr(self.__sample_rate)
                + "\n}\n")

    @property
    def text(self):
        return self.__text

    @property
    def audio(self):
        return self.__audio

    @property
    def length(self):
        return self.__length

    @property
    def sample_rate(self):
        return self.__sample_rate


