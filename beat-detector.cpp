// ***** BEATDETECTOR.H *****
#ifndef BeatDetectorH
#define BeatDetectorH

class TBeatDetector
{
private:
  float KBeatFilter;        // Filter coefficient
  float Filter1Out, Filter2Out;
  float BeatRelease;        // Release time coefficient
  float PeakEnv;            // Peak enveloppe follower
  bool BeatTrigger;         // Schmitt trigger output
  bool PrevBeatPulse;       // Rising edge memory
public:
  bool BeatPulse;           // Beat detector output

  TBeatDetector();
  ~TBeatDetector();
  virtual void setSampleRate(float SampleRate);
  virtual void AudioProcess (float input);
};
#endif


// ***** BEATDETECTOR.CPP *****
#include "BeatDetector.h"
#include "math.h"

#define FREQ_LP_BEAT 150.0f    // Low Pass filter frequency
#define T_FILTER 1.0f/(2.0f*M_PI*FREQ_LP_BEAT)  // Low Pass filter time constant
#define BEAT_RTIME 0.02f  // Release time of enveloppe detector in second

TBeatDetector::TBeatDetector()
// Beat detector constructor
{
  Filter1Out=0.0;
  Filter2Out=0.0;
  PeakEnv=0.0;
  BeatTrigger=false;
  PrevBeatPulse=false;
  setSampleRate(44100);
}

TBeatDetector::~TBeatDetector()
{
  // Nothing in the deconstructor
}

void TBeatDetector::setSampleRate (float sampleRate)
// Compute all sample frequency related coefficients
{
  KBeatFilter=1.0/(sampleRate*T_FILTER);
  BeatRelease=(float)exp(-1.0f/(sampleRate*BEAT_RTIME));
}

void TBeatDetector::AudioProcess (float input)
// Process incoming signal
{
  float EnvIn;

  // Note: Perhaps It is better taking the abolute value before the filter?

  // Step 1 : 2nd order low pass filter (made of two 1st order RC filter)
  Filter1Out=Filter1Out+(KBeatFilter*(input-Filter1Out));
  Filter2Out=Filter2Out+(KBeatFilter*(Filter1Out-Filter2Out));

  // Step 2 : peak detector
  EnvIn=fabs(Filter2Out);
  if (EnvIn>PeakEnv) PeakEnv=EnvIn;  // Attack time = 0
  else
  {
    PeakEnv*=BeatRelease;
    PeakEnv+=(1.0f-BeatRelease)*EnvIn;
  }

  // Step 3 : Schmitt trigger
  if (!BeatTrigger)
  {
    if (PeakEnv>0.3) BeatTrigger=true;
  }
  else
  {
    if (PeakEnv<0.15) BeatTrigger=false;
  }

  // Step 4 : rising edge detector
  BeatPulse=false;
  if ((BeatTrigger)&&(!PrevBeatPulse))
    BeatPulse=true;
  PrevBeatPulse=BeatTrigger;
}
