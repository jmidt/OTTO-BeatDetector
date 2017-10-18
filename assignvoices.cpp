bool detectHits = true;
bool hitDetected = false;
int vdx = 0; //Number of assigned voices.
auto &&vd = props.voiceData[vdx];
if(detectHits) {
  for (int sn = 0; sn<rs; ++sn) {
    BeatDetector.AudioProcess(sampleData[sn]);
    hitDetected = BeatDetector.BeatPulse;
    if (hitDetected) {
      if (vdx > 0 && vd.out >= rs) { //Not the first hit
        vd.out = sn - shiftAmount; // Go back so we avoid beginning of next hit   
        auto &&vd = props.voiceData[vdx]; //Next voice
      }
      if (vd.in < 0) {
        vd.in = std::max(sn - shiftAmount, 0); // Go back a little, so we get entire hit, but only to beginning
        vdx++;
      }
      hitDetected = false;
    }
  } 
  if (vd.in >= 0) { //If start has been set, set the end at the end of the sample.
    vd.out = rs-1;
}
//Set remaining voices
for (int i = vdx; i < nVoices; ++i) {
      auto &&vd = props.voiceData[i];
      if (vd.in < 0 || vd.out >= rs) {
        vd.in = i * (rs / nVoices);
        vd.out = (i + 1) * rs / nVoices;
      }
}
