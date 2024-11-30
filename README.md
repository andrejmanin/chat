# **Project: «Name»**
Проєкт «Name» побудований на мові С++ та бібліотеці Boost.Asio. Базується на асинхронному мережевому програмуванні. Це основа для майбутнього розширення в щось більше за консольний додаток.

# **Основний функціонал (v 1.0.2):**
-	Додаток дозволяє переписуватися один з одним.

## Статус проєкту:
🔧 в розробці - Інтерфейс для клієнта;  
🔧 в розробці - Шифрування даних;  
✅ завершено - Логер для сервера;  
✅ завершено - Передача/отримання повідомлень;  

## Вимоги до проєкту:
🛠️	Мова програмування: С++ 17;  
📚 Бібліотека: Boost.Asio;  
⚙️ Редактор коду (рекомендовано: CLion);  

# Інструкція встановлення:
## Встановлення бібліотеки Boost.Asio (Linux version):

**Ubuntu:**  

```console
sudo apt update
sudo apt install libboost-all-dev
```

**Fedora:**  

```console
sudo dnf update
sudo dnf install boost-devel
```

**CentOS:**  
Якщо ви використовуєте CentOS 8 або новішої версії, вам може знадобитися ввімкнути репозиторій PowerTools, щоб отримати бібліотеки розробки Boost:

```console
sudo yum config-manager --set-enabled powertools
```

```console
sudo yum install boost-devel
sudo yum update
```

**Arch:**  

```console
sudo yum update
sudo pacman -S boost
```

Коли встановлення буде завершено, бібліотеки Boost будуть встановлені у вашій системі, як правило, в /usr. Щоб переконатися, що бібліотеки Boost встановлено правильно, ви можете перевірити номер версії:

`cat /usr/include/boost/version.hpp | grep "BOOST_LIB_VERSION"`  

Якщо вам потрібно більше інформації зверніться за цим джерелом:  
-	https://www.boost.io/doc/user-guide/getting-started.html  

Вся основна інформація щодо встановлення бібліотеки була взята звідси.

## Встановлення проєкту:  

```console 
git clone https://github.com/andrejmanin/chat.git
```  

## Компіляція програми:  

```console
cd chat/
mkdir build 
cd build
cmake .. -D BOOST_ROOT=path/to/boost_1_85_0   
cmake --build .  
```

1.	Створіть каталог збірки, у якому повинні зберігатися результати 
2.	Змініть поточний робочий каталог для будування проєкту 
3.	Налаштуйте CMake, надавши йому посилання на каталог Boost 
4.	Збудуйте проект

**Якщо ви використовуєте CMake >=3.13, це можна спростити до:**  
```console
cmake -S . -B build -D BOOST_ROOT=path/to/boost_1_85_0   
cmake --build build  
```
## **CMakeLists.txt:**
```CMake
find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(client
        client/core/client.cpp
        client/core/client.h
        client/main.cpp
        parameters.h
)

add_executable(server
        server/core/server.cpp
        server/core/server.h
        server/main.cpp
        parameters.h
        server/core/session.cpp
        server/core/session.h
)



target_link_libraries(server ${Boost_LIBRARIES})
target_link_libraries(client ${Boost_LIBRARIES})
```

## **Будування проєкту:**  
```console
cd build/
make client
make server
```

## **Запуск програми:**
```console
./server
```
 
`server started`  
  
```console
./client
```

`Connected to the server`  
`Enter username:`

## Зміна адреси підключення клієнта
Проєкт завантажується за **ip адресою: 127.0.0.1 (localhost)**. Для редагування підключення клієнта потрібно перейти в файл **client.h**, параметр **IP_ADDR**. За потреби змінюєте на ту адресу яка потрібна.

## Додаткові посилання для ознайомлення:
-	Встановлення бібліотеки Boos.Asio: https://www.boost.io/doc/user-guide/getting-started.html122:46
-	Документація бібліотеки: https://www.boost.org/doc/libs/1_84_0/doc/html/boost_asio.html


