//
// Created by lang liu on 24-12-13.
//
//

#include <ev++.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>

void Message(AMQP::TcpChannel* channel, const AMQP::Message& message, uint64_t deliveryTag, bool redelivered)
{
    std::string msg;
    msg.assign(message.body(), message.bodySize());
    std::cout << msg << std::endl;
    channel->ack(deliveryTag);
}


int main()
{
    auto* loop = EV_DEFAULT;

    AMQP::LibEvHandler handler(loop);

    AMQP::Address address("amqp://root:123456@127.0.0.1:5762/");
    AMQP::TcpConnection connection(&handler, address);

    AMQP::TcpChannel channel(&connection);

    channel.declareExchange("test-exchange", AMQP::ExchangeType::direct)
        .onError([](const char* message) {
            std::cout << "声明交换机失败：" << message << std::endl;
            exit(EXIT_FAILURE);
        })
        .onSuccess([](){
            std::cout << "test-exchange 交换机创建成功!" << std::endl;
        });
    
    channel.declareQueue("test-queue")
        .onError([](const char* message) {
            std::cout << "test-queue 队列创建失败" << std::endl;
            exit(EXIT_FAILURE);
        })
        .onSuccess([](){
            std::cout << "test-exchange 队列创建成功!" << std::endl;
            exit(EXIT_FAILURE);
        });

            //6. 针对交换机和队列进行绑定
    channel.bindQueue("test-exchange", "test-queue", "test-queue-key")
        .onError([](const char *message) {
            std::cout << "test-exchange - test-queue 绑定失败：" << message << std::endl;
            exit(0);
        })
        .onSuccess([](){
            std::cout << "test-exchange - test-queue 绑定成功！" << std::endl;
        });


    for (size_t i = 0; i < 10; i++)
    {
        std::string msg = "Hello Bite-" + std::to_string(i);
        bool ret = channel.publish("test-exchange", "test-queue-key", msg);
        if (!ret)
        {
            std::cout << "publish 失败!\n";
        }
    }
    
    ev_run(loop);

    return 0;
}
