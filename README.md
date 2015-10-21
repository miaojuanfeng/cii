# cii
Codeigniter内置框架

### 简介

### 性能

### CII

```
root@mjf-virtual-machine:/home/mjf/httpd/support# ./ab -n10 -c1 http://localhost/cii/index.php
This is ApacheBench, Version 2.3 <$Revision: 1638069 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient).....done


Server Software:        nginx/1.9.3
Server Hostname:        localhost
Server Port:            80

Document Path:          /cii/index.php
Document Length:        2636 bytes

Concurrency Level:      1
Time taken for tests:   0.065 seconds
Complete requests:      10
Failed requests:        0
Total transferred:      27970 bytes
HTML transferred:       26360 bytes
Requests per second:    152.68 [#/sec] (mean)
Time per request:       6.550 [ms] (mean)
Time per request:       6.550 [ms] (mean, across all concurrent requests)
Transfer rate:          417.04 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       0
Processing:     3    6   5.7      3      18
Waiting:        3    6   5.7      3      18
Total:          3    7   5.7      3      18

Percentage of the requests served within a certain time (ms)
  50%      3
  66%      6
  75%      8
  80%     16
  90%     18
  95%     18
  98%     18
  99%     18
 100%     18 (longest request)
```

```
root@mjf-virtual-machine:/home/mjf/httpd/support# ./ab -n10 -c1 http://localhost/cii/index.php
This is ApacheBench, Version 2.3 <$Revision: 1638069 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient).....done


Server Software:        nginx/1.9.3
Server Hostname:        localhost
Server Port:            80

Document Path:          /cii/index.php
Document Length:        2636 bytes

Concurrency Level:      1
Time taken for tests:   0.069 seconds
Complete requests:      10
Failed requests:        0
Total transferred:      27970 bytes
HTML transferred:       26360 bytes
Requests per second:    144.13 [#/sec] (mean)
Time per request:       6.938 [ms] (mean)
Time per request:       6.938 [ms] (mean, across all concurrent requests)
Transfer rate:          393.68 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       0
Processing:     3    7   7.7      3      27
Waiting:        3    6   7.7      3      27
Total:          3    7   7.7      3      27

Percentage of the requests served within a certain time (ms)
  50%      3
  66%      3
  75%      9
  80%     13
  90%     27
  95%     27
  98%     27
  99%     27
 100%     27 (longest request)
```

```
root@mjf-virtual-machine:/home/mjf/httpd/support# ./ab -n10 -c1 http://localhost/cii/index.php
This is ApacheBench, Version 2.3 <$Revision: 1638069 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient).....done


Server Software:        nginx/1.9.3
Server Hostname:        localhost
Server Port:            80

Document Path:          /cii/index.php
Document Length:        2636 bytes

Concurrency Level:      1
Time taken for tests:   0.060 seconds
Complete requests:      10
Failed requests:        0
Total transferred:      27970 bytes
HTML transferred:       26360 bytes
Requests per second:    166.22 [#/sec] (mean)
Time per request:       6.016 [ms] (mean)
Time per request:       6.016 [ms] (mean, across all concurrent requests)
Transfer rate:          454.03 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.0      0       0
Processing:     3    6   4.1      6      15
Waiting:        0    6   4.4      6      15
Total:          3    6   4.1      6      15

Percentage of the requests served within a certain time (ms)
  50%      6
  66%      6
  75%      9
  80%     10
  90%     15
  95%     15
  98%     15
  99%     15
 100%     15 (longest request)
```

```
root@mjf-virtual-machine:/home/mjf/httpd/support# ./ab -n10 -c1 http://localhost/cii/index.php
This is ApacheBench, Version 2.3 <$Revision: 1638069 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient).....done


Server Software:        nginx/1.9.3
Server Hostname:        localhost
Server Port:            80

Document Path:          /cii/index.php
Document Length:        2636 bytes

Concurrency Level:      1
Time taken for tests:   0.058 seconds
Complete requests:      10
Failed requests:        0
Total transferred:      27970 bytes
HTML transferred:       26360 bytes
Requests per second:    171.57 [#/sec] (mean)
Time per request:       5.828 [ms] (mean)
Time per request:       5.828 [ms] (mean, across all concurrent requests)
Transfer rate:          468.64 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       0
Processing:     2    6   5.6      4      17
Waiting:        0    5   5.8      3      16
Total:          2    6   5.6      4      17

Percentage of the requests served within a certain time (ms)
  50%      4
  66%      4
  75%      6
  80%     15
  90%     17
  95%     17
  98%     17
  99%     17
 100%     17 (longest request)
```

```
root@mjf-virtual-machine:/home/mjf/httpd/support# ./ab -n10 -c1 http://localhost/cii/index.php
This is ApacheBench, Version 2.3 <$Revision: 1638069 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient).....done


Server Software:        nginx/1.9.3
Server Hostname:        localhost
Server Port:            80

Document Path:          /cii/index.php
Document Length:        2636 bytes

Concurrency Level:      1
Time taken for tests:   0.051 seconds
Complete requests:      10
Failed requests:        0
Total transferred:      27970 bytes
HTML transferred:       26360 bytes
Requests per second:    197.15 [#/sec] (mean)
Time per request:       5.072 [ms] (mean)
Time per request:       5.072 [ms] (mean, across all concurrent requests)
Transfer rate:          538.49 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       0
Processing:     3    5   3.1      3      12
Waiting:        1    5   3.3      3      12
Total:          3    5   3.1      3      12

Percentage of the requests served within a certain time (ms)
  50%      3
  66%      6
  75%      8
  80%      8
  90%     12
  95%     12
  98%     12
  99%     12
 100%     12 (longest request)
 ```
### CI

```
root@mjf-virtual-machine:/home/mjf/httpd/support# ./ab -n10 -c1 http://localhost/ci/index.php
This is ApacheBench, Version 2.3 <$Revision: 1638069 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient).....done


Server Software:        nginx/1.9.3
Server Hostname:        localhost
Server Port:            80

Document Path:          /ci/index.php
Document Length:        74671 bytes

Concurrency Level:      1
Time taken for tests:   0.324 seconds
Complete requests:      10
Failed requests:        4
   (Connect: 0, Receive: 0, Length: 4, Exceptions: 0)
Total transferred:      748314 bytes
HTML transferred:       746704 bytes
Requests per second:    30.86 [#/sec] (mean)
Time per request:       32.401 [ms] (mean)
Time per request:       32.401 [ms] (mean, across all concurrent requests)
Transfer rate:          2255.44 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       0
Processing:    25   32  12.3     28      65
Waiting:       24   30  10.2     25      57
Total:         25   32  12.3     28      66

Percentage of the requests served within a certain time (ms)
  50%     28
  66%     30
  75%     35
  80%     36
  90%     66
  95%     66
  98%     66
  99%     66
 100%     66 (longest request)
 ```
 
 cii 的执行速度是 ci 的5倍
