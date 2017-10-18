struct Beat {
   int in_point;
   float weight;
};
std::vector<Beat> beats;
bool detectHits = true;
bool hitDetected = false;
int shiftAmount = 10; //Test by trial and error

//Find threshold for detection
// Calculate standard deviation, mean is 0
float sigm = 0;
for (int sn = 0; sn<rs; ++sn) {
   sigm += fabs(sampleData[sn])/sqrt(rs-1);
}
  
//Find hits. push_back to beats.
if(detectHits) {
  for (int sn = 0; sn<rs; ++sn) { //Goes through all samples in the buffer
    BeatDetector.AudioProcess(sampleData[sn]);
    hitDetected = BeatDetector.BeatPulse;
    if (hitDetected) {
      Beat cur_beat = {
        std::max(sn-shiftAmount,0), //Go back a little. 
        BeatDetector.getWeight()}; //Get the weight of the current hit somehow.
      beats.push_back(cur_beat);
    }
  } 
}
//Sort to get the nVoices (24?) largest hits.
std::sort(std::begin(beats), std::end(beats),  
  [] (Beat& l, Beat& r) {
    return l.weight > r.weight;
  });
int n_found = std::min(nVoices, beats.size());
std::sort(std::begin(beats), std::begin(beats) + n_found,
  [] (Beat& l, Beat& r) {
    return l.in_point < r.in_point;
  });
for (int i = 0; i < n_found; i++) {
  voice[i].in = beats[i].in_point;
  if (i < n_found-1)
    voice[i].out = beats[i+1].in_point; //Set end-point to beginning of next big hit.
  else
    voice[i].out = rs-1; //Set last voice to end of sample
}
//Set remaining voices to default
int n_default = nVoices - n_found;
int default_length = sampleBuffer.size() / n_default;
for (int i = 0; i < n_default; i++) {
  voice[n_found + i].in = i * default_length;
}
