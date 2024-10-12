#include <bits/stdc++.h>
using namespace std;
using pii = pair<int, int>;

#define DEBUG 0

const int dy[] = { -1, 0, 1, 0 };
const int dx[] = { 0, 1, 0, -1 };

struct cmp {
	bool operator()(int& a, int& b) {
		return (!(a > b));
	}
};

typedef struct {
	int id;
	int y;
	int x;
	int power;
	int flag;
	int gun; 
} Node;

Node tnode;

class Solution {
public:
	vector<int> answer;
	int N, M, K;

	vector<vector<priority_queue<int, vector<int>, cmp> > > MAP;
	vector<vector<int> > POS;
	vector<Node> HUMAN;

	bool printArr(vector<vector<priority_queue<int, vector<int>, cmp> > >& T, string s = "") {
		cout << " ===== " << s << " =====\n";
		for (int y = 0; y < N; ++y)	{
			for (int x = 0; x < N; ++x) {
				priority_queue<int, vector<int>, cmp>& pq = T[y][x];
				if (!pq.empty())
					printf("%3d(%d) ", pq.top(), pq.size());
				else printf("%3d(0) ", 0);
			}
				
			putchar('\n');
		}
		putchar('\n');

		return (true);
	}

	bool printArr(vector<vector<int> >& T, string s = "") {
		cout << " ===== " << s << " =====\n";
		for (int y = 0; y < N; ++y)	{
			for (int x = 0; x < N; ++x) {
				if (T[y][x] != -1)
					printf("%3d(%d) ", T[y][x], HUMAN[T[y][x]].flag);
				else
					printf("%3d    ", -1);
			}
				
			putchar('\n');
		}
		putchar('\n');

		return (true);
	}

	bool printNode(int id) {
		Node& t = HUMAN[id];
		printf("id: %d  y: %d, x: %d, flag: %d, power: %d, gun: %d\n", id, t.y, t.x, t.flag, t.power, t.gun);

		return (true);
	}

	bool printAllNode() {
		for (int i = 0; i < M; ++i)	printNode(i);
		return (true);
	}

	bool printPQ(int y, int x) {
		printf("(%d, %d): ", y, x);
		priority_queue<int, vector<int>, cmp> pq = MAP[y][x];
		while (!pq.empty())	{
			cout << pq.top();	pq.pop();
		}
		putchar('\n');
		
		return (true);
	}

	inline bool isValid(int y, int x) {
		return (!(y < 0 || x < 0 || y >= N || x >= N));
	}

	void run() {
		for (int id = 0; id < M; ++id) {
			Node& node = HUMAN[id];	// [1]
			int ny = node.y + dy[node.flag];	
			int nx = node.x + dx[node.flag];

			if (!isValid(ny, nx)) {	// [1-1]
				node.flag = (node.flag + 2) % 4;
				ny = node.y + dy[node.flag];
				nx = node.x + dx[node.flag];
			}

			DEBUG && printf("%d 사람이 (%d, %d) -> (%d, %d) 좌표로 이동 예정\n", id, node.y, node.x, ny, nx);

			POS[node.y][node.x] = -1;
			node.y = ny;
			node.x = nx;

			if (POS[ny][nx] == -1) {	// [2] 이동한 칸에 플레이어가 없다면
				DEBUG && cout << "이동 칸에 플레이어 없음\n";
				auto& pq = MAP[ny][nx];

				if (!pq.empty()) {	// [2] 총이 있는지 확인
					if (node.gun < pq.top()) {	// [2-1]
						if (node.gun != 0) pq.push(node.gun);	// [2-2]
						node.gun = pq.top();	// [2-1]
						pq.pop();
					}
				}
				POS[node.y][node.x] = id;
			}
			else {	// [3]
				auto& pq = MAP[ny][nx];
				int cmpId = POS[ny][nx];
				Node& cmp = HUMAN[cmpId];
				bool chk;

				if (node.power + node.gun > cmp.power + cmp.gun)	// [3-1]
					chk = true;	//이동한 사람이 이김
				else if (node.power + node.gun < cmp.power + cmp.gun)
					chk = false;	//기존 사람이 이김
				else {	//비기는 경우, 두 명의 초기 power가 겹치는 경우는 없음
					if (node.power > cmp.power)	chk = true;
					else	chk = false;
				}

				if (chk) {	//이동한 사람이 이김
					DEBUG && printf("(%d, %d) 좌표에서 충돌, 이동한 사람이 이김\n", ny, nx);
					answer[id] += (node.power - cmp.power) + (node.gun - cmp.gun);	// [3-2]

					if (cmp.gun != 0) { // [3-3] 총 내려 놓음
						MAP[ny][nx].push(cmp.gun);	
						cmp.gun = 0;
					}

					for (int cycle = 0; cycle < 4; ++cycle) {
						ny = cmp.y + dy[cmp.flag];
						nx = cmp.x + dx[cmp.flag];

						if (isValid(ny, nx) && POS[ny][nx] == -1) // [3-3-1] 다른 플레이어가 없고 격자 안인 경우 이동 
							break;

						cmp.flag = (cmp.flag + 1) % 4;
					}

					cmp.y = ny;
					cmp.x = nx;
					POS[node.y][node.x] = id;
					POS[cmp.y][cmp.x] = cmpId;

					if (!MAP[cmp.y][cmp.x].empty())	{	// [3-3-2]
						cmp.gun = MAP[cmp.y][cmp.x].top();
						MAP[cmp.y][cmp.x].pop();
					}

					if (!MAP[node.y][node.x].empty()) {
						if (node.gun < MAP[node.y][node.x].top()) {
							if (node.gun != 0) pq.push(node.gun);
							node.gun = pq.top();
							pq.pop();
						}
					}
					
				}
				else {	//기존 사람이 이김
					DEBUG && printf("(%d, %d) 좌표에서 충돌, 기존 사람이 이김\n", ny, nx);
					answer[cmpId] += (cmp.power - node.power) + (cmp.gun - node.gun);	// [3-2]

					if (node.gun != 0) { // [3-3] 총 내려 놓음
						MAP[ny][nx].push(node.gun);	
						node.gun = 0;
					}
                    
					for (int cycle = 0; cycle < 4; ++cycle) {
						ny = node.y + dy[node.flag];
						nx = node.x + dx[node.flag];

						if (isValid(ny, nx) && POS[ny][nx] == -1) // [3-3-1] 다른 플레이어가 없고 격자 안인 경우 이동 
							break;

						node.flag = (node.flag + 1) % 4;
					}

					node.y = ny;
					node.x = nx;
					POS[node.y][node.x] = id;
					POS[cmp.y][cmp.x] = cmpId;

					if (!MAP[node.y][node.x].empty())	{	// [3-3-2]
						node.gun = MAP[node.y][node.x].top();
						MAP[node.y][node.x].pop();
					}

					if (!MAP[cmp.y][cmp.x].empty()) {
						if (cmp.gun < MAP[cmp.y][cmp.x].top()) {
							if (cmp.gun != 0) pq.push(cmp.gun);
							cmp.gun = pq.top();
							pq.pop();
						}
					}

				}
			}
			DEBUG && printArr(POS, to_string(id) + " 사람이 이동한 후 상태");
			DEBUG && printAllNode();
		}
	}

	void solution() {
		DEBUG && printArr(MAP, "초기 총 상태");
		DEBUG && printArr(POS, "초기 사람 위치");
		DEBUG && printAllNode();

		/**
		 * 1. 첫 번재 플레이어 부터 본인의 방향으로 1칸 이동
		 * 	1-1. 격자를 벗어나는 경우 반대 방향으로 바꾸어 1칸 이동
		 * 2. (이동한 칸에 플레이어가 없다면) 해당 칸에 총 있는지 확인.
		 * 	2-1. 가지고 있는 총과 놓여져 있는 총들 중 가장 쎈 총을 선택
		 * 	2-2. 나머지는 자리에 둠
		 * 3. (이동한 방향에 플레이어가 있다면) 싸움 시작
		 * 	3-1. [플레이어 초기 능력치 + Gun 공격력 합] 비교하여 큰 플레이어가 이김. 수치가 같으면 초기 능력치가 높은 사람이 Win.
		 * 	3-2. 승리자는 각 플레이어의 [초기 능력치와 Gun 공격력] 합의 차이만큼 포인트 획득. answer에 갱신
		 * 	3-3. 진 사람은 본인이 가지고 있는 총 내려놓고 플레이어의 가는 방향으로 1칸 이동
		 * 		3-3-1. 이동하려는 칸에 다른 플레이어가 있거나 격자 밖인 경우 오른쪽으로 90도씩 회전하며 빈 칸으로 이동.
		 * 		3-3-2. 이동한 칸에 총이 있다면, 가장 공격력이 높은 총을 획득
		 * 	... [3-3] 작업 후
		 * 		3-2-1. 이긴 플레이어는 떨어져 있는 총과 내 총 중 가장 쌘 Gun을 획득. 
		 */

		for (int k = 1; k <= K; ++k) {
			DEBUG && cout << " >>>>> " << k << " 턴 시작\n";
			run();
			DEBUG && printArr(MAP, to_string(k) + " 턴 후 MAP 상태");
			DEBUG && printArr(POS, to_string(k) + " 턴 후 사람 상태");


		//	for (int i = 0; i < M; ++i) cout << answer[i] << " ";
		//	putchar('\n');
		//	putchar('\n');
		}
	
	}

	void input() {
		cin >> N >> M >> K;

		answer.resize(M);

		MAP.resize(N, vector<priority_queue<int, vector<int>, cmp> >(N));
		for (int y = 0; y < N; ++y) 
			for (int x = 0; x < N; ++x) {
				int t;	cin >> t;
				if (t == 0)	continue;
				MAP[y][x].push(t);
			}
	
		HUMAN.resize(M);
		POS.resize(N, vector<int>(N, -1));
		tnode.gun = 0;
		for (int i = 0; i < M; ++i) {
			cin >> tnode.y >> tnode.x >> tnode.flag >> tnode.power;
			--tnode.y;
			--tnode.x;
			tnode.id = i;
			HUMAN[i] = tnode;
			POS[tnode.y][tnode.x] = i;
		}
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);

	Solution s;
	s.input();
	s.solution();
	int len = s.answer.size();
	for (int i = 0; i < len; ++i) cout << s.answer[i] << " ";
}