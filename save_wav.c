#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    int sampleRate;   // サンプリングレート
    int bitDepth;     // ビット深度
    int channels;     // チャンネル数
    double duration;  // 持続時間（秒）
    double frequency; // サイン波の周波数（Hz）
} WavSettings;

void writeWavHeader(FILE *file, WavSettings settings, int dataSize) {
    int chunkSize = 36 + dataSize;
    int subchunk1Size = 16;
    short audioFormat = 1; // PCM
    short bitsPerSample = settings.bitDepth;
    int byteRate = settings.sampleRate * settings.channels * bitsPerSample / 8;
    short blockAlign = settings.channels * bitsPerSample / 8;

    fwrite("RIFF", 1, 4, file);
    fwrite(&chunkSize, 4, 1, file);
    fwrite("WAVE", 1, 4, file);
    fwrite("fmt ", 1, 4, file);
    fwrite(&subchunk1Size, 4, 1, file);
    fwrite(&audioFormat, 2, 1, file);
    fwrite(&settings.channels, 2, 1, file);
    fwrite(&settings.sampleRate, 4, 1, file);
    fwrite(&byteRate, 4, 1, file);
    fwrite(&blockAlign, 2, 1, file);
    fwrite(&bitsPerSample, 2, 1, file);
    fwrite("data", 1, 4, file);
    fwrite(&dataSize, 4, 1, file);
}

void initializeWavFile(const char *filename, WavSettings settings) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("ファイルを開けません");
        exit(1);
    }

    int numSamples = (int)(settings.duration * settings.sampleRate);
    int dataSize = numSamples * settings.channels * settings.bitDepth / 8;

    writeWavHeader(file, settings, dataSize);

    fclose(file);
}

void generateSineWave(const char *filename, WavSettings settings) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("ファイルを開けません");
        exit(1);
    }

    int numSamples = (int)(settings.duration * settings.sampleRate);
    int dataSize = numSamples * settings.channels * settings.bitDepth / 8;

    writeWavHeader(file, settings, dataSize);

    double amplitude = pow(2, settings.bitDepth - 1) - 1; // 最大振幅を計算

    for (int i = 0; i < numSamples; i++) {
        double sample = sin(2 * M_PI * settings.frequency * i / settings.sampleRate);
        short int sampleValue = (short int)(sample * amplitude);
        fwrite(&sampleValue, sizeof(sampleValue), 1, file);
    }

    fclose(file);
    printf("サイン波のオーディオデータが書き込まれました: %s\n", filename);
}

int main() {
    WavSettings settings = {44100, 16, 1, 5.0, 440.0};

    initializeWavFile("sine_wave.wav", settings);

    generateSineWave("sine_wave.wav", settings);

    return 0;
}
