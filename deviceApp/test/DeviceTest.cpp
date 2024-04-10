#include "Device.h"

#include "MockSerialPort.h"

#include <fakeit.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

#include <string>

using namespace fakeit;

TEST_CASE("Check default Device parameters")
{
    Mock<SerialPortInterface> mock;
    SerialPortInterface& serialPort = mock.get();

    When(Method(mock, setPortName)).Return();
    When(Method(mock, setBaudRate)).Return();
    When(Method(mock, setDataBits)).Return();
    When(Method(mock, setParity)).Return();
    When(Method(mock, setStopBits)).Return();

    //    When(Method(mock, open)).Return(true);
    When(Method(mock, isOpen)).Return(false);

    When(Method(mock, baudRate)).Return(QSerialPort::Baud9600);
    When(Method(mock, parity)).Return(QSerialPort::NoParity);
    When(Method(mock, dataBits)).Return(QSerialPort::Data8);
    When(Method(mock, stopBits)).Return(QSerialPort::OneStop);

    //    MockSerialPort serialPort;
    //    Device device(serialPort);

    // given
    Device device(serialPort);

    // when
    const auto parameters = device.getParameters();

    //    Verify(Method(mock, setPortName));
    //    Verify(Method(mock, setBaudRate).Using(QSerialPort::BaudRate::Baud9600));
    //    Verify(Method(mock, setDataBits).Using(QSerialPort::DataBits::Data8));
    //    Verify(Method(mock, setParity).Using(QSerialPort::Parity::NoParity));
    //    Verify(Method(mock, setStopBits).Using(QSerialPort::StopBits::OneStop));

    //    Verify(Method(mock, isOpen));
    //    Verify(Method(mock, open));

    // expected
    CHECK(parameters.frequency == 2);
    CHECK(parameters.debug == false);
    CHECK(parameters.BaudRate == QSerialPort::BaudRate::Baud9600);
    CHECK(parameters.DataBits == QSerialPort::DataBits::Data8);
    CHECK(parameters.Parity == QSerialPort::Parity::NoParity);
    CHECK(parameters.StopBits == QSerialPort::StopBits::OneStop);
}

//TEST_CASE("Mock serial port")
//{
//
//    Mock<SomeInterface> mock;
//
//    When(Method(mock,foo)).Return(0);
//
//    SomeInterface &i = mock.get();
//
//    // Production code
//    i.foo(1);
//
//    // Verify method mock.foo was invoked.
//    Verify(Method(mock,foo));
//
//    // Verify method mock.foo was invoked with specific arguments.
//    Verify(Method(mock,foo).Using(2));
//
//    // given
//    Device device;
//
//    // when
//    const auto parameters = device.getParameters();
//
//    // expected
//    CHECK(parameters.frequency == 2);
//    CHECK(parameters.debug == false);
//    CHECK(parameters.BaudRate == QSerialPort::BaudRate::Baud9600);
//    CHECK(parameters.DataBits == QSerialPort::DataBits::Data8);
//    CHECK(parameters.Parity == QSerialPort::Parity::NoParity);
//    CHECK(parameters.StopBits == QSerialPort::StopBits::OneStop);
//
//}
