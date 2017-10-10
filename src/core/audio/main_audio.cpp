#include "main_audio.hpp"
#include "core/globals.hpp"
#include "util/algorithm.hpp"

namespace top1::audio {

  // ProcessData<2> Mixer::process_tracks(ProcessData<4>);
  // ProcessData<2> Mixer::process_engine(ProcessData<1>);
  // ProcessData<4> Tapedeck::playback(ProcessData<0>);
  // ProcessData<0> Tapedeck::record(ProcessData<1>);
  // ProcessData<1> Synth::process(ProcessData<0>);
  // ProcessData<1> Effect::process(ProcessData<1>);
  // ProcessData<0> Sequencer::process(ProcessData<0>);
  // ProcessData<0> Modulation::process(ProcessData<0>);
  // ProcessData<2> MasterEffect::process(ProcessData<2>);

  ProcessData<2> MainAudio::process(ProcessData<1> external_in)
  {
    using Selection = modules::InputSelector::Selection;

    // Main processor function
    auto midi_in = external_in.midi_only();
    auto playback_out = Globals::tapedeck.process_playback(midi_in);
    auto mixer_out = Globals::mixer.process_tracks(playback_out);

    auto record_in = [&] () {
      switch (Globals::selector.props.input.get()) {
      case Selection::Internal:
        return Globals::effect.process(Globals::synth.process(midi_in));
      case Selection::External:
        return Globals::effect.process(external_in);
      case Selection::TrackFB:
        util::transform(playback_out, audiobuf1.begin(),
          [track = Globals::selector.props.track.get()] (auto&& a) {
            return std::array{a[track]};
          });
        return external_in.redirect(audiobuf1);
      case Selection::MasterFB:
        break;
      }
      return audio::ProcessData<1>{{nullptr},{nullptr}};
    }();

    if (Globals::selector.props.input != Selection::MasterFB) {
      Globals::mixer.process_engine(record_in);
    }

    // mixer_out = Globals::master_fx.process(mixer_out);

    if (Globals::selector.props.input == Selection::MasterFB) {
      util::transform(mixer_out, audiobuf1.begin(),
        [] (auto&& a) { return std::array{a[0] + a[1]}; });
      record_in = {{audiobuf1.data(), external_in.nframes}, external_in.midi};
    }

    Globals::tapedeck.process_record(record_in);

    return mixer_out;
  }

}