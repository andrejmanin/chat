#ifndef INTERFACE_H
#define INTERFACE_H


namespace Interface {
    class Colors {
    public:
        static void send();
        static void receive();
        static void info();
        static void error();
        static void resetColor();
    };

    class Info {
    public:
        static void start();
        static void registerUser();
        static void loginUser();
        static void registrationSuccess();
        static void loginSuccess();
        static void close();
        static void getClients();
        static void sentUsername();
        static void info();
        static void msgInfo();
    };
}



#endif //INTERFACE_H
