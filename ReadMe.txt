/*Read me */ (본 플젝은, 트래버스 ,딜리트에 쓰레드를 적용하였음)

submitVER_6_thread: 컴퓨팅 파워나, cpu의 상태에 따라 성능차가 심한 것을 알 수 있었음. cpu상태가 좋을 경우 성능 좋음
submitVER_4_thread: 쓰레드 4개 이용, 평균적으로 균일한 성능 개선을 보여줌
submitVER_2_thread: 쓰레드 2개 이용 성능개선 미미함

-

skiplist

skip_list_origin.c
- 쓰레드를 안쓴 순수한 스킵리스트, 서치속도 매우 빠름(1/10로 줄음), 그러나 인서트와 트래버스가 느림
- 결국 이번 프로젝트인 insert, traverse, delete 성능에서는 delete만 성능이 크게 좋아져서 데모때 쓰지 않았음.

skipl_list.c
- 쓰레드를 사용한 스킵리스트, 서치속도가 기존 스킵리스트보다 빨랐으나 insert 오버헤드가 커져서 개발중단.
- 딜리트는 미구현
                      