/* https://github.com/huguesjohnson/DubbelLib/blob/main/LICENSE */

/*
BinToWav - see https://huguesjohnson.com/programming/java/BinToWav/
*/

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.nio.file.Files;
import java.nio.file.Paths;

import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;

//this is not meant to be enterprise-y code - it's quick utility class to for a specific purpose
//this was written and tested in 45 minutes - most of that time spent debating whether I wanted to do any error
// handling at all
public abstract class BinToWav {
    private final static String[] folderList =
        {"Test", "Talker 0", "Talker 1", "Talker 2", "Talker 3", "Talker 4", "Talker 5", "Talker 6", "Talker 7"};
    private final static String SEPARATOR = File.separator;
    //these settings assume the .bin is a redbook audio file
    private final static float SAMPLE_RATE = 40000;
    private final static int SAMPLE_BITS = 16;
    private final static int CHANNELS = 1;
    private final static boolean SIGNED = true;
    private final static boolean BIG_ENDIAN = false;
    private final static AudioFormat FORMAT = new AudioFormat(SAMPLE_RATE, SAMPLE_BITS, CHANNELS, SIGNED, BIG_ENDIAN);

    // this expects two arguments
    // inputPath = path to .bin files
    // outputPath = path (directory) to write wav files
    public static void binToWav(String inputPath, String outputPath) {
        AudioInputStream audioIn = null;
        String currentPath;
        List<String> files;

        // get all files
        for (String folder : folderList) {
            currentPath = inputPath + "/" + folder;
            new File(outputPath + "/" + folder).mkdirs();

            files = Stream.of(new File(currentPath).listFiles())
                .filter(file -> !file.isDirectory() && file.getName().endsWith(".BIN")).map(File::getName).toList();

            for (String fileName : files) {
                try {
                    // now get all the bytes - I'll reserve commentary on the weirdness of not having an overload for
                    // getAllBytes that takes a string
                    byte[] bytes = Files.readAllBytes(Paths.get(currentPath + "/" + fileName));
                    audioIn = new AudioInputStream((new ByteArrayInputStream(bytes)), FORMAT,
                        bytes.length / FORMAT.getFrameSize());

                    //build the output file name
                    String outputFile = outputPath + "/" + folder + "/" + fileName.replace(".BIN", ".WAV");
                    AudioSystem.write(audioIn, AudioFileFormat.Type.WAVE, new File(outputFile));
                    audioIn.close();
                } catch (Exception x) {
                    x.printStackTrace();
                } finally {
                    //don't care about errors closing open files
                    if (audioIn != null) {
                        try {
                            audioIn.close();
                        } catch (Exception x) {
                        }
                    }
                }
            }
        }
    }

    public static void main(String[] args) {
        binToWav("/home/toor/projects/IST-201/audio-samples/crm",
            "/home/toor/projects/IST-201/audio-samples/crm/WAV");
    }
}