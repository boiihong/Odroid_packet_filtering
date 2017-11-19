Odroid_packet_filtering

these project implement packet filtering and testing environment.

1. hw2_client(android)	

android client 파일이다. 실행하면 hello world! 이외의 출력은 아무것도 없으나,
thread에서 자동적으로 serverName 변수에 해당하는 ip로 연결을 하여 패킷을 전송한다. 
--먼저 String serverName 을 내가 원하는 서버의 ip 주소로 변경해야 한다. 

2. hw2_client 

일반 desktop에서 실행 가능한 java 파일이다. 이 또한 serverName 변수를 내가 원하는 ip
주소로 세팅을 해 주어야 한다.

3. hw2_server(android)

서버의 app 이다. odroid에서 실행 되며, 다른 기기에서 실행하더라도 수정할 사항은 없다.

4. hw2_02.c

리눅스 커널 모듈이다. LKM에서 내가 필터링 할(server app 에서 버튼을 누름으로서 packet을 더이상 들어오지 않게 할)
mac 주소를 세팅 해 주어야 한다. line 126은 desktop 에 해당하는 mac 주소를 기입하고 line 140에는 또다른 기기에 대한
mac주소를 기입한 후에, 컴파일, insmod 하면 된다. 

5.hw2_02.ko

내 pc 와 내 휴대폰의 mac 주소를 필터링 함으로 의미가 없는 파일이다. 하지만 첨부한다. 

6.result

필터링 결과의 console 내용이다.
