#include <cpputils/result.hpp>
#include <gtest/gtest.h>

using namespace cu;

TEST(result_tests, returns_correct_result_and_message)
{
    std::string expectedMessage{"msg"};
    bool        expectedResult{false};

    Result res{expectedResult, expectedMessage};

    auto actualMessage{res.message()};
    auto actualResult{res.succeeded()};

    EXPECT_EQ(actualMessage, expectedMessage);
    EXPECT_EQ(actualResult, expectedResult);
}

TEST(result_tests, copy_semantics_work)
{
    Result resOriginal{false, "message"};
    Result resCopyConstructed{resOriginal};
    Result resCopyAssigned;
    resCopyAssigned = resOriginal;

    EXPECT_EQ(resOriginal.succeeded(), resCopyConstructed.succeeded());
    EXPECT_EQ(resOriginal.message(), resCopyConstructed.message());

    EXPECT_EQ(resOriginal.succeeded(), resCopyAssigned.succeeded());
    EXPECT_EQ(resOriginal.message(), resCopyAssigned.message());
}

TEST(result_tests, move_semantics_work)
{
    Result resOriginal{false, "message"};
    Result resOriginal1{resOriginal};
    Result resOriginal2{resOriginal};
    Result resMoveConstructed{std::move(resOriginal1)};
    Result resMoveAssigned;
    resMoveAssigned = std::move(resOriginal2);

    EXPECT_EQ(resOriginal.succeeded(), resMoveConstructed.succeeded());
    EXPECT_EQ(resOriginal.message(), resMoveConstructed.message());

    EXPECT_EQ(resOriginal.succeeded(), resMoveAssigned.succeeded());
    EXPECT_EQ(resOriginal.message(), resMoveAssigned.message());
}

TEST(dataresult_tests, returns_correct_data)
{
    int              val;
    DataResult<int&> res{val};

    EXPECT_EQ(&val, &res.data());
}

TEST(dataresult_tests, throws_when_getting_null_data)
{
    DataResult<int&> res;

    EXPECT_THROW({ res.data(); }, std::runtime_error);
}

TEST(response_tests, returns_correct_status_and_data)
{
    std::string expectedStatus{"msg"};
    int         val;
    int*        expectedAddress{&val};

    Response<std::string, int&> res{expectedStatus, val};

    auto& actualStatus{res.status()};
    auto  actualAddress{&res.data()};

    EXPECT_EQ(expectedStatus, actualStatus);
    EXPECT_EQ(expectedAddress, actualAddress);
}

TEST(response_tests, throws_when_getting_null_data)
{
    Response<std::string, int&> res{""};

    EXPECT_THROW({ res.data(); }, std::runtime_error);
}

TEST(status_tests, returns_correct_status_and_message)
{
    std::string expectedMessage{"msg"};
    int         expectedStatus{4};

    Status<int> status{expectedStatus, expectedMessage};

    auto& actualMessage{status.message()};
    auto  actualStatus{status.status()};

    EXPECT_EQ(expectedMessage, actualMessage);
    EXPECT_EQ(expectedStatus, actualStatus);
}

TEST(datastatus_tests, returns_correct_values)
{
    std::string expectedMessage{"msg"};
    int         expectedStatus{4};
    int         v;
    int*        expectedAddress{&v};

    DataStatus<int, int&> status{expectedStatus, v, expectedMessage};

    auto& actualMessage{status.message()};
    auto  actualStatus{status.status()};
    auto  actualAddress{&status.data()};

    EXPECT_EQ(expectedMessage, actualMessage);
    EXPECT_EQ(expectedStatus, actualStatus);
    EXPECT_EQ(expectedAddress, actualAddress);
}

TEST(datastatus_tests, throws_when_getting_null_data)
{
    DataStatus<int, int&> status{3};

    EXPECT_THROW({ status.data(); }, std::runtime_error);
}

TEST(resultcollector_tests,
     anyFailed_returns_true_when_at_least_one_failure_exists)
{
    Result res1;
    Result res2;
    Result res3{false};
    Result res4;

    ResultCollector collector;

    collector.addResult(res1);
    collector.addResult(res2);
    collector.addResult(res3);
    collector.addResult(res4);

    EXPECT_TRUE(collector.anyFailed());
}

TEST(resultcollector_tests, anyFailed_returns_false_when_no_failure_exists)
{
    Result res1;
    Result res2;
    Result res3;
    Result res4;

    ResultCollector collector;

    collector.addResult(res1);
    collector.addResult(res2);
    collector.addResult(res3);
    collector.addResult(res4);

    EXPECT_FALSE(collector.anyFailed());
}

TEST(resultcollector_tests,
     anySucceeded_returns_true_when_at_least_one_success_exists)
{
    Result res1{false};
    Result res2{false};
    Result res3;
    Result res4{false};

    ResultCollector collector;

    collector.addResult(res1);
    collector.addResult(res2);
    collector.addResult(res3);
    collector.addResult(res4);

    EXPECT_TRUE(collector.anySucceeded());
}

TEST(resultcollector_tests, anySucceeded_returns_false_when_no_success_exists)
{
    Result res1{false};
    Result res2{false};
    Result res3{false};
    Result res4{false};

    ResultCollector collector;

    collector.addResult(res1);
    collector.addResult(res2);
    collector.addResult(res3);
    collector.addResult(res4);

    EXPECT_FALSE(collector.anySucceeded());
}

TEST(resultcollector_tests, non_empty_messages_gets_added_to_list)
{
    Result res1{true, "msg"};
    Result res2{true, ""};
    Result res3{true, "msg"};
    Result res4{true, "msg"};
    Result res5{true};

    ResultCollector collector;

    collector.addResult(res1);
    collector.addResult(res2);
    collector.addResult(res3);
    collector.addResult(res4);
    collector.addResult(res5);

    EXPECT_EQ(3, collector.messages().size());
}

TEST(statusactionmapper_tests, execute_calls_binded_callback)
{
    StatusActionMapper<std::string> mapper;
    bool                            status1Executed{};
    bool                            status2Executed{};
    bool                            status3Executed{};

    mapper.bind("status 1", [&status1Executed] { status1Executed = true; });
    mapper.bind("status 2", [&status2Executed] { status2Executed = true; });
    mapper.bind("status 3", [&status3Executed] { status3Executed = true; });

    mapper.execute("status 2");

    EXPECT_FALSE(status1Executed);
    EXPECT_TRUE(status2Executed);
    EXPECT_FALSE(status3Executed);
}
