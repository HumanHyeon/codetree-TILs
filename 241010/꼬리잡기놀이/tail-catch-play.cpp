/**
 * 이 문제에서 잘못한 점
 * 1. 팀 구성할 때, BFS가 아닌 DFS로 짤 것
 * 2. MAP에 추가하지말고 Section 관리 배열을 따로 만들 것
 */

#include <bits/stdc++.h>
using namespace std;
using pii = pair<int, int>;

#define DEBUG 0

const int dy[] = { 0, -1, 0, 1 };
const int dx[] = { 1, 0, -1, 0 };

enum DIRECT { RIGHT, UP, LEFT, DOWN };

typedef struct {
	int y;
	int x;
	int cnt;
} Node;

class Solution {
public:
	int answer;
	int N, M, K;
	vector<vector<pii> > MAP;
	vector<deque<pii> > TEAM;
	pii BALL;
	int direct;

	Solution() : answer(0) {};

	bool printArr(vector<vector<pii> >& T, string s = "") {
		cout << " ===== " << s << " =====\n";
		for (int y = 0; y < N; ++y) {
			for (int x = 0; x < N; ++x)
				printf("%2d ", T[y][x].first);
			putchar('\n');
		}
		putchar('\n');

		return (true);
	}

	bool printTeam() {
		for (int i = 0; i < TEAM.size(); ++i) {
			cout << "TEAM " << i << ": ";
			auto& team = TEAM[i];
			for (auto& t : team)
				cout << " (" << t.first << "," <<t.second << ") ";
			putchar('\n');
		}

		return (true);
	}

	inline bool isValid(int y, int x) {
		return (!(y < 0 || x < 0 || y >= N || x >= N));
	}

	inline void changeHead(deque<pii>& T) {
		MAP[T.front().first][T.front().second].first = 3;
		MAP[T.back().first][T.back().second].first = 1;
		reverse(begin(T), end(T));
	}

	/** 
	 * TODO: 매개변수로 받은 id에 해당하는 팀을 앞으로 전진한다.
	 * deque와 MAP에 모두 상태를 갱신한다.
	 */
	void moveTeam(int id) {
		const pii& head = TEAM.at(id).front();
		pii newHead = make_pair(-1, -1);

		DEBUG && cout << "ID: " << id << " 팀의 현재 Head y: " << head.first << " x: " << head.second << "\n";
		for (int flag = 0; flag < 4; ++flag) {
			int ny = head.first + dy[flag];
			int nx = head.second + dx[flag];

			if (!isValid(ny, nx))	continue;
			if (MAP[ny][nx].first == 0 || MAP[ny][nx].first == 2)	continue;

			newHead = make_pair(ny, nx);
			break;
		}

		DEBUG && cout << "find new head y: " << newHead.first << " x: " << newHead.second << "\n";

		MAP[TEAM.at(id).back().first][TEAM.at(id).back().second].first = 4;	// [1.3] TAIL(삭제)
		TEAM.at(id).pop_back();	// [1.2]
		MAP[TEAM.at(id).back().first][TEAM.at(id).back().second].first = 3;	// [1.3] 새로운 TAIL(변경)
		MAP[head.first][head.second].first = 2;	// [1.3] 기존 HEAD(변경)
		MAP[newHead.first][newHead.second].first = 1; // [1.3] HEAD(추가)
		MAP[newHead.first][newHead.second].second = id;
		TEAM.at(id).push_front(newHead);	// [1.1]	
	}

	/**
	 * TODO: 매개변수인 현재 턴을 받아 이번 턴에 던질 좌표를 BALL에 Update 
	 */
	void setBall(int turn) {
		turn %= (N * 4);
		if (0 <= turn && turn < N) {
			BALL.first = turn % N;
			BALL.second = 0;
			direct = RIGHT;
		}
		else if (N <= turn && turn < 2 * N) {
			BALL.first = N - 1;
			BALL.second = turn % N;
			direct = UP;
		}
		else if (2 * N <= turn && turn < 3 * N) {
			BALL.first = N - (turn % N) - 1;
			BALL.second = N - 1;
			direct = LEFT;
		}
		else {
			BALL.first = 0;
			BALL.second = N - (turn % N) - 1;
			direct = DOWN;
		}
	}

	/**
	 * TODO: BALL에 대한 정보와 direct 방향으로 공을 던지고 점수를 계산한다.
	 * MAP에 대한 정보로만 판단.
	 */
	int throwBall() {
		pii t = make_pair(-1, -1);
		queue<Node> q;
		vector<vector<bool> > visited(N, vector<bool>(N, false));
		Node node;
		int dist;

		if (direct == RIGHT) {
			int y = BALL.first;
			for (int x = BALL.second; x < N; ++x) 
				if (MAP[y][x].first == 1 || MAP[y][x].first == 2 || MAP[y][x].first == 3) {
					t = make_pair(y, x);
					break;
				}
		}
		else if (direct == UP) {
			int x = BALL.second;
			for (int y = BALL.first; y >= 0; --y)
				if (MAP[y][x].first == 1 || MAP[y][x].first == 2 || MAP[y][x].first == 3) {
					t = make_pair(y, x);
					break;
				}
		}
		else if (direct == LEFT) {
			int y = BALL.first;
			for (int x = BALL.second; x >= 0; --x)
				if (MAP[y][x].first == 1 || MAP[y][x].first == 2 || MAP[y][x].first == 3) {
					t = make_pair(y, x);
					break;
				}
		}
		else {
			int x = BALL.second;
			for (int y = BALL.first; y < N; ++y)
				if (MAP[y][x].first == 1 || MAP[y][x].first == 2 || MAP[y][x].first == 3) {
					t = make_pair(y, x);
					break;
				}
		}

		if (t.first == -1) return (0);

		DEBUG && cout << "Hit Pos y: " << t.first << " x: " << t.second << "\n";

		int id = MAP[t.first][t.second].second;
		deque<pii>& team = TEAM.at(id);
		int len = team.size();

		for (int i = 0; i < len; ++i) {
			pii& cmp = team.at(i);

			if (cmp.first == t.first && cmp.second == t.second) {
				dist = i + 1;
				break;
			}
		}
		changeHead(TEAM.at(id));

		return (dist * dist);
	}

	void solution() {
		DEBUG && cout << " ~~~~~~~ 초기 Status ~~~~~~~\n";
		DEBUG && printArr(MAP, "초기 MAP");
		DEBUG && printTeam();
		DEBUG && cout << " ~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

		/** 
		 * 1. 머리사람 따라 앞으로 전진 (모든 팀)
		 * 	1.1 머리사람 앞으로 전진, deque에 push_front
		 * 	1.2 꼬리사람 삭제, deque에 pop_back
		 * 	1.3 MAP에 갱신. HEAD(추가), TAIL(삭제), 기존 HEAD(변경), 새로운 TAIL(변경)
		 * 2. 공을 던짐, 공 사이클 수 = 4 * N
		 * 3.(y, x) 공 진로에 있는 가장 첫 번째 사람 Hit
		 * 	3.1 맞으면, 머리사람을 기준으로 몇 번째에 있는지 확인. k라면 k * k 만큼 점수 획득
		 * 	3.2 공을 획득한 팀은 머리사람과 꼬리사람이 바뀜 (reverse)
		 * ...
		 * 4. (1 ~ 3)을 K 번 반복 후 획득한 점수 총합 answer에 Update
		 */

		for (int k = 0; k < K; ++k) {
			int score;

			for (int i = 0; i < M; ++i)	moveTeam(i);	// [1]
			DEBUG && printArr(MAP, "이동 후 MAP 상태");
			DEBUG && printTeam();
			setBall(k);	// [2]
			DEBUG && cout << "이번 턴에 던질 시작 공 좌표: (" << BALL.first << "," << BALL.second << ")" << " direct: " << direct << "\n"; 
			score = throwBall();
			answer += score;
			DEBUG && printArr(MAP, "던진 후의 MAP, 이번 턴에 얻은 점수: " + to_string(score) + "  총 점수: " + to_string(answer));
			DEBUG && printTeam();
		}
	}

	void setTeam(int id) {
		deque<pii>& team = TEAM[id];
		queue<pii> q;
		vector<vector<bool> > visited(N, vector<bool>(N, false));
		bool chk = false;
		
		q.push(team.front());
		visited[team.front().first][team.front().second] = true;
		while (!q.empty()) {
			int y = q.front().first;
			int x = q.front().second;
			q.pop();

			for (int flag = 0; flag < 4; ++flag) {
				int ny = y + dy[flag];
				int nx = x + dx[flag];

				if (!isValid(ny, nx))	continue;
				if (visited[ny][nx] || MAP[ny][nx].first == 0 || MAP[ny][nx].first == 4)	continue;
				if (!chk && MAP[ny][nx].first == 3)	continue;

				q.push(make_pair(ny, nx));
				visited[ny][nx] = true;
				team.push_back(make_pair(ny, nx));
				MAP[ny][nx].second = id;

				if (MAP[ny][nx].first == 3)	return;
				chk = true;
				break;
			}
		}
	}

	void input() {
		int idx = 0;

		cin >> N >> M >> K;

		MAP.resize(N, vector<pii>(N));
		TEAM.resize(M, deque<pii>());
		for (int y = 0; y < N; ++y)	for (int x = 0; x < N; ++x) {
			cin >> MAP[y][x].first;
			if (MAP[y][x].first == 1) {
				MAP[y][x].second = idx;
				TEAM[idx++].push_back(make_pair(y, x));
			}
		}

		for (int i = 0; i < M; ++i) setTeam(i);
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);

	Solution s;
	s.input();
	s.solution();
	cout << s.answer;	
}