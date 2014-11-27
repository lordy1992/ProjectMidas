#include "GestureSeqRecorder.h"
#include <gtest\gtest.h>
#include <Windows.h>

// register one pose sequence, progress with that sequence in that mode, and verify
// response is what was registered.
TEST(gestSeqRecorder, basicRegisterAndRespond)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;
    
    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    SharedCommandData *scd = new SharedCommandData();
    ControlState cs = ControlState(scd);
    cs.setMode(midasMode::LOCK_MODE);

    sequenceResponse response;
    ss = gsr.progressSequence(Pose::Type::thumbToPinky, cs, response);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    ASSERT_EQ(response.responseType, ResponseType::STATE_CHANGE);
    ASSERT_EQ(response.responseAction.mode, midasMode::GESTURE_MODE);
}

// register a two pose sequence, progress with that sequence in that mode, and verify
// response is what was registered. Then, repeat the process and ensure the same thing
// is returned. This will ensure that the state is reset properly after it's first success.
TEST(gestSeqRecorder, basicRegisterAndRespond2)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::fist);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    SharedCommandData *scd = new SharedCommandData();
    ControlState cs = ControlState(scd);
    cs.setMode(midasMode::LOCK_MODE);

    sequenceResponse response;
    // Trial one!
    ss = gsr.progressSequence(Pose::Type::thumbToPinky, cs, response);
    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    ASSERT_EQ(response.responseType, ResponseType::NONE);
    ss = gsr.progressSequence(Pose::Type::fist, cs, response);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    ASSERT_EQ(response.responseType, ResponseType::STATE_CHANGE);
    ASSERT_EQ(response.responseAction.mode, midasMode::GESTURE_MODE);

    // Trial two! Ensure everything was reset properly
    ss = gsr.progressSequence(Pose::Type::thumbToPinky, cs, response);
    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    ASSERT_EQ(response.responseType, ResponseType::NONE);
    ss = gsr.progressSequence(Pose::Type::fist, cs, response);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    ASSERT_EQ(response.responseType, ResponseType::STATE_CHANGE);
    ASSERT_EQ(response.responseAction.mode, midasMode::GESTURE_MODE);
}

// register two pose sequences, progress with each sequence in that mode, and verify
// response is what was registered.
TEST(gestSeqRecorder, basicRegisterAndRespond3)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::fingersSpread);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);
    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    // Register second sequence, with different response info.
    sequence seq2;
    seq2.push_back(Pose::Type::thumbToPinky);
    seq2.push_back(Pose::Type::fist);
    sequenceResponse seqResp2;
    seqResp2.responseType = ResponseType::STATE_CHANGE;
    seqResp2.responseAction.mode = midasMode::GESTURE_MODE;

    gsr.registerSequence(midasMode::LOCK_MODE, seq2, seqResp2);
    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    // Activate each sequence and verify response
    SharedCommandData *scd = new SharedCommandData();
    ControlState cs = ControlState(scd);
    cs.setMode(midasMode::LOCK_MODE);

    // Test first sequence
    sequenceResponse response;
    ss = gsr.progressSequence(Pose::Type::thumbToPinky, cs, response);
    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    ASSERT_EQ(response.responseType, ResponseType::NONE);
    ss = gsr.progressSequence(Pose::Type::fingersSpread, cs, response);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    ASSERT_EQ(response.responseType, seqResp1.responseType);
    ASSERT_EQ(response.responseAction.mode, seqResp1.responseAction.mode);

    // Test second sequence
    ss = gsr.progressSequence(Pose::Type::thumbToPinky, cs, response);
    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    ASSERT_EQ(response.responseType, ResponseType::NONE);
    ss = gsr.progressSequence(Pose::Type::fist, cs, response);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    ASSERT_EQ(response.responseType, seqResp2.responseType);
    ASSERT_EQ(response.responseAction.mode, seqResp2.responseAction.mode);

}

// Register 2, size-1, unique sequences for the same mode. ensure success
TEST(gestSeqRecorder, passRegister1)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    sequence seq2;
    seq2.push_back(Pose::Type::fingersSpread);

    ss = gsr.registerSequence(midasMode::LOCK_MODE, seq2, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
}

// Register 2, size=2, unique sequences for the same mode. ensure success
TEST(gestSeqRecorder, passRegister2)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::thumbToPinky);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    sequence seq2;
    seq2.push_back(Pose::Type::thumbToPinky);
    seq2.push_back(Pose::Type::fingersSpread);

    ss = gsr.registerSequence(midasMode::LOCK_MODE, seq2, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
}

// Register 2, size>=2, unique sequences for the same mode with sizes not equal (executes different branches depending on which is larger).
// Ensure success
TEST(gestSeqRecorder, passRegister3)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::fingersSpread);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    sequence seq2;
    seq2.push_back(Pose::Type::thumbToPinky);
    seq2.push_back(Pose::Type::fingersSpread);

    ss = gsr.registerSequence(midasMode::LOCK_MODE, seq2, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
}

// Register 2, size>=2, unique sequences for the same mode with sizes not equal (executes different branches depending on which is larger).
// Other chosen to be larger than in passRegister3 test. Ensure success
TEST(gestSeqRecorder, passRegister4)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::thumbToPinky);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    sequence seq2;
    seq2.push_back(Pose::Type::thumbToPinky);
    seq2.push_back(Pose::Type::fingersSpread);
    seq2.push_back(Pose::Type::thumbToPinky);

    ss = gsr.registerSequence(midasMode::LOCK_MODE, seq2, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);
}

// Pass simple registration in sepearte modes, which would conflict if in same mode.
TEST(gestSeqRecorder, passRegister5)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = SequenceStatus::INVALID_SEQUENCE;
    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt != midasMode::GESTURE_MODE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        ss = gsr.registerSequence(mm, seq1, seqResp1);

        ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    }
}

// Fail by conflicting with 2 identical, size=1 sequences.
TEST(gestSeqRecorder, failRegister1)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::CONFLICTING_SEQUENCE);
}

// Fail by conflicting with 2 identical, size>1 sequences.
TEST(gestSeqRecorder, failRegister2)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::fingersSpread);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::CONFLICTING_SEQUENCE);
}

// Fail by conflicting with 2 non-identical, size>1 sequences, with first sequence smaller.
TEST(gestSeqRecorder, failRegister3)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::fingersSpread);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    //Increase size!
    seq1.push_back(Pose::Type::fist);
    ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::CONFLICTING_SEQUENCE);
}

// Fail by conflicting with 2 non-identical, size>1 sequences, with first sequence larger.
TEST(gestSeqRecorder, failRegister4)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::fingersSpread);
    seq1.push_back(Pose::Type::fist);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::SUCCESS);

    //Decrease size!
    seq1.pop_back();
    ss = gsr.registerSequence(midasMode::LOCK_MODE, seq1, seqResp1);

    ASSERT_EQ(ss, SequenceStatus::CONFLICTING_SEQUENCE);
}

// Corner case 1 - ensure that registering into various modes the first time 
// works, but will fail when registering illegal values again. The difference 
// here is that other succesful registrations will take place in between first
// success and fail.
TEST(gestSeqRecorder, failRegisterCorner1)
{
    GestureSeqRecorder gsr = GestureSeqRecorder(midasMode::LOCK_MODE, 100);

    sequence seq1;
    seq1.push_back(Pose::Type::thumbToPinky);
    seq1.push_back(Pose::Type::fist);
    seq1.push_back(Pose::Type::waveIn);
    seq1.push_back(Pose::Type::waveOut);
    seq1.push_back(Pose::Type::fingersSpread);
    sequenceResponse seqResp1;
    seqResp1.responseType = ResponseType::STATE_CHANGE;
    seqResp1.responseAction.mode = midasMode::GESTURE_MODE;

    SequenceStatus ss = SequenceStatus::INVALID_SEQUENCE;
    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt != midasMode::GESTURE_MODE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        ss = gsr.registerSequence(mm, seq1, seqResp1);

        ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    }

    // make seq1 longer, but with same base sequence (therefore conflicting).
    seq1.push_back(Pose::Type::thumbToPinky);
    // Re-register failing sequences (longer just to make it more interesting). Failures
    // should ensue.
    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt != midasMode::GESTURE_MODE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        ss = gsr.registerSequence(mm, seq1, seqResp1);

        ASSERT_EQ(ss, SequenceStatus::CONFLICTING_SEQUENCE);
    }

    // make seq1 shorter, but with same base sequence (therefore conflicting).
    seq1.pop_back();
    seq1.pop_back();
    // Re-register failing sequences (longer just to make it more interesting). Failures
    // should ensue.
    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt != midasMode::GESTURE_MODE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        ss = gsr.registerSequence(mm, seq1, seqResp1);

        ASSERT_EQ(ss, SequenceStatus::CONFLICTING_SEQUENCE);
    }

    // register new ending to seq1, which should make it successful! (effectively replacing the 
    // fingersSpread from the first registration).
    seq1.push_back(Pose::Type::thumbToPinky);
    // Re-register failing sequences (longer just to make it more interesting). Failures
    // should ensue.
    for (int midasModeInt = midasMode::LOCK_MODE; midasModeInt != midasMode::GESTURE_MODE; midasModeInt++)
    {
        midasMode mm = static_cast<midasMode>(midasModeInt);
        ss = gsr.registerSequence(mm, seq1, seqResp1);

        ASSERT_EQ(ss, SequenceStatus::SUCCESS);
    }
}