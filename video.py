import sys
import time
import cv2
import os

class Converter:
    """Base converter class, contains the asciify() method which is used in all Converter subclasses."""

    def asciify(self, image):
        for row in image:
            for b, g, r in row:
                lumination = 0.2126 * r + 0.7152 * g + 0.0722 * b
                index = int((lumination / 255) * (self._gradient_len - 1))
                yield self.gradient[index]+self.gradient[index]

            yield "\n"



class VideoConverter(Converter):
    """A converter class to handle converting videos and gifs to ASCII."""

    def __init__(self, filename: str, scale: float, width_stretch: float, gradient: str = "░▒▓█", loop: bool = False):
        if not os.path.isfile(filename):
            raise FileNotFoundError(filename)

        self.filename = filename
        self.scale = scale
        self.width_stretch = width_stretch
        self.gradient = list(gradient)
        self.loop = loop

        self._gradient_len = len(gradient)
        self._video = cv2.VideoCapture(filename)
        self._fps = self._video.get(cv2.CAP_PROP_FPS)
        self._width = self._video.get(3)
        self._height = self._video.get(4)
        self._scaled_dims = (
            round(self._width * self.scale * self.width_stretch),
            round(self._height * self.scale),
        )
        #self._line_breaks = ("\n" * (os.get_terminal_size().lines - self._scaled_dims[1])) + "\r"
        self._line_breaks = "\n"

        self.ascii_frames = []

    def convert(self):
        while True:
            success, frame = self._video.read()

            if not success:
                break

            frame = cv2.resize(frame, self._scaled_dims).tolist()
            #print(frame)
            self.ascii_frames.append("".join(self.asciify(frame)))

        return self

    def view(self, fps: float = None):
        if fps is None:
            spf = 1 / self._fps
        else:
            spf = 1 / fps

        try:
            while True:
                for frame in self.ascii_frames:
                    start = time.time()
                    #print(self._line_breaks + frame, end="")
                    print(frame)
                    print("\n")
                    time.sleep(spf - (start - time.time()))

                if not self.loop:
                    break
        except KeyboardInterrupt:
            print()

orig_stdout = sys.stdout
f = open('out.txt', 'w', encoding="utf-8")
sys.stdout = f
print("░▒▓█\n")
v = VideoConverter('myvideo360.mp4', .1, True)
v.convert()  # convert the frames into ascii
v.view(37.5)  # play the converted frames in the console
