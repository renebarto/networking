#include "GoogleTest.h"

#include "osal/Signal.h"

namespace osal {

class SignalTest : public ::testing::Test
{
public:
    SignalType m_signalValue;
    bool m_hadSignal;

    SignalTest()
        : m_signalValue()
        , m_hadSignal()
    {

    }
    void SignalHandler(SignalType signal)
    {
        SignalTest::m_signalValue = signal;
        SignalTest::m_hadSignal = true;
    }
};

TEST_F(SignalTest, GenerateSigAbort)
{
    SetSignalHandler(SignalType::Abort, std::bind(&SignalTest::SignalHandler, this, std::placeholders::_1));

    EXPECT_EQ(SignalType::None, m_signalValue);
    EXPECT_FALSE(m_hadSignal);

    Raise(SignalType::Abort);

    EXPECT_EQ(SignalType::Abort, m_signalValue);
    EXPECT_TRUE(m_hadSignal);

    ResetSignalHandler(SignalType::Abort);
}

TEST_F(SignalTest, GenerateSigIll)
{
    SetSignalHandler(SignalType::Illegal, std::bind(&SignalTest::SignalHandler, this, std::placeholders::_1));

    EXPECT_EQ(SignalType::None, m_signalValue);
    EXPECT_FALSE(m_hadSignal);

    Raise(SignalType::Illegal);

    EXPECT_EQ(SignalType::Illegal, m_signalValue);
    EXPECT_TRUE(m_hadSignal);

    ResetSignalHandler(SignalType::Illegal);
}

TEST_F(SignalTest, GenerateSigInt)
{
    SetSignalHandler(SignalType::Interrupt, std::bind(&SignalTest::SignalHandler, this, std::placeholders::_1));

    EXPECT_EQ(SignalType::None, m_signalValue);
    EXPECT_FALSE(m_hadSignal);

    Raise(SignalType::Interrupt);

    EXPECT_EQ(SignalType::Interrupt, m_signalValue);
    EXPECT_TRUE(m_hadSignal);

    ResetSignalHandler(SignalType::Interrupt);
}

TEST_F(SignalTest, GenerateSigSegv)
{
    SetSignalHandler(SignalType::SegmentViolation, std::bind(&SignalTest::SignalHandler, this, std::placeholders::_1));

    EXPECT_EQ(SignalType::None, m_signalValue);
    EXPECT_FALSE(m_hadSignal);

    Raise(SignalType::SegmentViolation);

    EXPECT_EQ(SignalType::SegmentViolation, m_signalValue);
    EXPECT_TRUE(m_hadSignal);

    ResetSignalHandler(SignalType::SegmentViolation);
}

TEST_F(SignalTest, GenerateSigTerm)
{
    SetSignalHandler(SignalType::Terminate, std::bind(&SignalTest::SignalHandler, this, std::placeholders::_1));

    EXPECT_EQ(SignalType::None, m_signalValue);
    EXPECT_FALSE(m_hadSignal);

    Raise(SignalType::Terminate);

    EXPECT_EQ(SignalType::Terminate, m_signalValue);
    EXPECT_TRUE(m_hadSignal);

    ResetSignalHandler(SignalType::Terminate);
}

} // namespace osal
