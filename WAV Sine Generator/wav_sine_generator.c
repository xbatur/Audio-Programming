#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/*Standard values for CD-quality audio*/
#define SUBCHUNK1SIZE   (16)
#define AUDIO_FORMAT    (1) /*For PCM*/
#define NUM_CHANNELS    (2)
#define SAMPLE_RATE     (44100)
#define BITS_PER_SAMPLE (16)
#define BYTE_RATE       (SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE / 8)
#define BLOCK_ALIGN     (NUM_CHANNELS * BITS_PER_SAMPLE / 8)

#define BUFF_SIZE (SAMPLE_RATE * 2) // 2 seconds
int16_t buffer[BUFF_SIZE];

typedef struct wav_file_header{
  char riff[4]; //Marks the file as a riff file. Characters are each 1 byte long.
  int32_t fileSize; //Size of the overall file - 8 bytes, in bytes (32-bit integer). Typically, you’d fill this in after creation.
  char wave[4]; //File Type Header. For our purposes, it always equals “WAVE”.
  char fmt[4]; //Format chunk marker. Includes trailing null
  int32_t formatDataLen; //Length of format data as listed above
  int16_t format; //Type of format (1 is PCM) - 2 byte integer
  int16_t numChannels; //Number of Channels - 2 byte integer
  int32_t sampleRate; //Sample Rate - 32 byte integer. Common values are 44100 (CD), 48000 (DAT). Sample Rate = Number of Samples per second, or Hertz.
  int32_t byteRate; // (Sample Rate * Bits Per Sample * Channels)/8
  int16_t blockAlign; //(BitsPerSample * Channels) / 8.1 - 8 bit mono2 - 8 bit stereo/16 bit mono4 - 16 bit stereo
  int16_t bitsPerSample; //Bits per sample
  char data[4]; //“data” chunk header. Marks the beginning of the data section.
  int32_t dataSize; //Size of the data section.
} wav_file_header;

void write_wav(char *filename, unsigned long num_samples, int16_t *data, int sample_rate){
  FILE *outFile;

  wav_file_header wav_header;
  memset(&wav_header,0,sizeof(wav_header));
  wav_header.riff[0] = 'R';
  wav_header.riff[1] = 'I';
  wav_header.riff[2] = 'F';
  wav_header.riff[3] = 'F';

  wav_header.wave[0] = 'W';
  wav_header.wave[1] = 'A';
  wav_header.wave[2] = 'V';
  wav_header.wave[3] = 'E';

  wav_header.fmt[0] = 'f';
  wav_header.fmt[1] = 'm';
  wav_header.fmt[2] = 't';
  wav_header.fmt[3] = ' ';
  
  wav_header.data[0] = 'd';
  wav_header.data[1] = 'a';
  wav_header.data[2] = 't';
  wav_header.data[3] = 'a';
  
  wav_header.formatDataLen = SUBCHUNK1SIZE;
  wav_header.format = AUDIO_FORMAT;
  wav_header.numChannels = NUM_CHANNELS;
  wav_header.sampleRate = SAMPLE_RATE;
  wav_header.byteRate = BYTE_RATE;
  wav_header.blockAlign = BLOCK_ALIGN;
  wav_header.bitsPerSample = BITS_PER_SAMPLE;
  wav_header.dataSize = num_samples * NUM_CHANNELS * 2;
  wav_header.fileSize = 36 + (BITS_PER_SAMPLE/2) * num_samples * NUM_CHANNELS;
  
  outFile = fopen(filename,"wb");
  assert(outFile);

  fwrite(&wav_header.riff,1,4,outFile);
  fwrite(&wav_header.fileSize,sizeof(int32_t),1,outFile);
  fwrite(&wav_header.wave,1,4,outFile);

  fwrite(&wav_header.fmt,1,4,outFile);
  fwrite(&wav_header.formatDataLen,sizeof(int32_t),1,outFile);
  fwrite(&wav_header.format,sizeof(int16_t),1,outFile);
  fwrite(&wav_header.numChannels,sizeof(int16_t),1,outFile);
  fwrite(&wav_header.sampleRate,sizeof(int32_t),1,outFile);
  fwrite(&wav_header.byteRate,sizeof(int32_t),1,outFile);
  fwrite(&wav_header.blockAlign,sizeof(int16_t),1,outFile);
  fwrite(&wav_header.bitsPerSample,sizeof(int16_t),1,outFile);

  fwrite(&wav_header.data,1,4,outFile);
  fwrite(&wav_header.dataSize,sizeof(int32_t),1,outFile);
  for(unsigned int i = 0; i < num_samples; i++){
    fwrite((int16_t*)(&data[i]),sizeof(int16_t),1,outFile);
  }
  fclose(outFile);
}

int main(int argc, char** argv){
  
  char* fileName = argv[2];
  float freqHZ = atol(argv[1]);
  float phase = 0;
  float amplitude = 32000;  
  float radPerSample = freqHZ*2*M_PI/SAMPLE_RATE;
  
  for(int i = 0; i < BUFF_SIZE; i++){
    phase += radPerSample;
    buffer[i] = (int)(amplitude * sin(phase));
  }

  write_wav(fileName,BUFF_SIZE,buffer,SAMPLE_RATE);
  return 0;
}
