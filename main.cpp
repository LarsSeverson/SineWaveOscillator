//
//  main.cpp
//  Writing Wave File - Audio
//
//  Created by Lars Severson on 1/4/23.
//

#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;


const int sampleRate = 44100;
const int bitDepth = 16;

class SineOscillator
{
public:
    SineOscillator(float f, float a) : frequency(f), amplitude(a){
        offset = (2 * M_PI * frequency) / sampleRate;
    }

    float process() {
        // Asin(2pif/sr)
        auto sample = amplitude * sin(angle);
        angle += offset;
        return sample;
    }

private:
    float frequency, amplitude, angle = 0.0f, offset = 0.0f;
};

void writeToFile(ofstream& file, int value, int size){
    file.write(reinterpret_cast<const char*>(&value), size);
}

int main() {

    ofstream audioFile;
    audioFile.open("waveform.wav", ios::binary);
    SineOscillator sineOscillator(440, 0.5);

    // Header chunk
    audioFile << "RIFF";
    audioFile << "----";
    audioFile << "WAVE";

    // Format chunk
    audioFile << "fmt ";
    writeToFile(audioFile, 16, 4); // size
    writeToFile(audioFile, 1, 2);  // compression code
    writeToFile(audioFile, 1, 2);  // number of channels
    writeToFile(audioFile, sampleRate, 4);  // sample rate
    writeToFile(audioFile, (sampleRate * bitDepth) / 8, 4); // byte rate
    writeToFile(audioFile, bitDepth / 8, 2); // block alignment
    writeToFile(audioFile, bitDepth, 2); // bit depth

    // Data chunk
    audioFile << "data";
    audioFile << "----";

    int preAudioPosition = (int)audioFile.tellp();
    auto maxAmplitude = pow(2, bitDepth - 1) - 1;
    // 2 seconds
    for(int i = 0; i < sampleRate * 2; i++){
        auto sample = sineOscillator.process();
        int intSample = static_cast<int>(sample * maxAmplitude);
        writeToFile(audioFile, intSample, 2);

    }
    int postAudioPosition = (int)audioFile.tellp();

    audioFile.seekp(preAudioPosition - 4);
    writeToFile(audioFile, postAudioPosition - preAudioPosition, 4);

    audioFile.seekp(4, ios::beg);
    writeToFile(audioFile, postAudioPosition - 8, 4);

    audioFile.close();
    return 0;
}
