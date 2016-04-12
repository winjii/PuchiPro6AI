#include <iostream>
#include <cstdio>
#include <vector>
#include <queue>
#include <cstdlib>
#include <climits>
using namespace std;

class Rand
{
private:
	static unsigned int x;
	static unsigned int y;
	static unsigned int z;
	static unsigned int w;
	static vector<int> weight;

	static struct Initializer
	{
		Initializer()
		{
			//using namespace chrono;
			//milliseconds t = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
			////cout << t.count() << " ms" << endl;
			//w = t.count() % UINT_MAX;
			////cout << w << endl;
			w = 1919810;
		}
	} initializer;
public:
	//[min, max)
	static int Next(int min, int max)
	{
		unsigned int t;
		t = x ^ (x << 11);
		x = y;
		y = z;
		z = w;
		w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
		return min + (double)w*(max - min) / ((double)UINT_MAX + 1);
	}
	//[0, max)
	static int Next(int max)
	{
		return Next(0, max);
	}
	static int WeightRand(int max)
	{
		if (weight.size() < 100) weight.resize(100);
		int sum = 0;
		for (int i = 0; i <= max; i++) sum += weight[i];
		int r = Next(sum);
		for (int i = 0; i <= max; i++)
		{
			r -= weight[i];
			if (r <= 0) return i;
		}
		return 0;
	}
};

unsigned int Rand::x = 123456789;
unsigned int Rand::y = 362436069;
unsigned int Rand::z = 521288629;
unsigned int Rand::w;
vector<int> Rand::weight = {49, 36, 25, 16, 9, 4};

const int HARD_OJAMA = -2;
const int NORMAL_OJAMA = -1;
const int EMPTY = 0;
//�F�̂������ʂ̋ʂ�, 1�ȏ�M�ȉ��̐����ŕ\�����

const int dx[4] = {0, 1, 0, -1}, dy[4] = {-1, 0, 1, 0}; //��, �E, ��, ��
const double DINF = 1e16;

class Point
{
public:
	int x, y;
	Point()
	{
		x = y = -1;
	}
	Point(int _x, int _y)
	{
		x = _x; y = _y;
	}
	//���W���w�肵��������1�}�X������
	void Move(int dir)
	{
		x += dx[dir]; y += dy[dir];
	}
	bool operator == (const Point &p) const { return x == p.x && y == p.y; }
};

class Cell
{
public:
	int kind;
	Cell()
	{
		kind = EMPTY;
	}
	Cell(int k)
	{
		kind = k;
	}
	static Cell Input()
	{
		int k;
		//���͎��s���̏I������
		if (!(cin >> k)) exit(0);
		return Cell(k);
	}
	bool IsOjama() const { return kind < 0; }
	bool IsHardOjama() const { return kind == -2; }
	bool IsNormalOjama() const { return kind == -1; }
	bool IsEmpty() const { return kind == 0; }
	bool IsColorful() const { return kind > 0; }
};

class OjamaCalculator
{
public:
	int weakness; //������܂���̉����ꂽ��
	int ojamaErasure; //������܂���������
	int colorfulErasure; //�F�t���ʂ���������
	OjamaCalculator()
	{
		weakness = ojamaErasure = colorfulErasure = 0;
	}
	OjamaCalculator(int _hardOjama, int _normalOjama, int _colorfulBall)
	{
		weakness = _hardOjama; ojamaErasure = _normalOjama; colorfulErasure = _colorfulBall;
	}
	bool IsHard() const
	{
		return colorfulErasure >= 35;
	}
	int Calculate() const
	{
		//TODO ����
		int ojamas = ojamaErasure + weakness;
		double k = 0.015;
		double l = 0.15;
		if (IsHard())
		{
			k *= 0.6;
			l *= 0.6;
		}
		return (int)(colorfulErasure*colorfulErasure*k + ojamas*ojamas*l);
	}
};

class State
{
public:
	int remainedTime;
	int W, H, N, M;
	//���W(x,y)�̏���field[x][y]�ɓ����Ă���
	//field[y][x]�ł͂Ȃ��̂Œ���!
	vector<vector<Cell> > field;
	//rain[x]�ɂ�, ��x�ɍ~��\��̋ʂ�\���������~�鏇�Ԃɋl�߂�
	vector<queue<int> > rain;

	State()
	{
		remainedTime = W = H = N = M = 0;
	}

	static State Input(int w, int h, int n, int m)
	{
		State res;
		res.W = w; res.H = h; res.N = n; res.M = m;
		res.field.resize(w);
		res.rain.resize(w);
		for (int x = 0; x < w; x++) res.field[x].reserve(h);

		cin >> res.remainedTime;
		for (int x = 0; x < w; x++)
		{
			int cnt;
			cin >> cnt;
			for (int i = 0; i < cnt; i++)
			{
				int ojama;
				cin >> ojama;
				res.rain[x].push(ojama);
			}
		}
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				res.field[x].push_back(Cell::Input());
			}
		}
		return res;
	}

	bool IsOutside(const Point &p) const { return p.x < 0 || W <= p.x || p.y < 0 || H <= p.y; }

private:
	void getLump(vector<Point> &ret, const Point &pos, int kind, vector< vector<bool> > &used)
		const
	{
		ret.push_back(pos);
		for (int i = 0; i < 4; i++)
		{
			Point next(pos);
			next.Move(i);
			if (IsOutside(next) || field[next.x][next.y].kind != kind || used[next.x][next.y])
				continue;
			used[next.x][next.y] = true;
			getLump(ret, next, kind, used);
		}
	}

public:
	//�w�肵�����W�̋ʂƊ��S�ɓ�����ނ̋ʂ̂����A�q�����Ă�����̂̍��W��Ԃ�(�w�肵�����W���܂�)
	//used�͊��ɒ��ׂ����W�̂Ƃ����true������
	vector<Point> GetLump(const Point &pos, vector< vector<bool> > &used) const
	{
		vector<Point> res;
		if (used[pos.x][pos.y]) return res;
		used[pos.x][pos.y] = true;
		getLump(res, pos, field[pos.x][pos.y].kind, used);
		return res;
	}
	//used�������ō���(�g���񂹂�Ƃ��͊O���Ő錾���Ďg���񂷂ق�������?)
	vector<Point> GetLump(const Point &pos) const
	{
		vector< vector<bool> > used(W);
		for (int i = 0; i < W; i++) used[i].resize(H, false);
		return GetLump(pos, used);
	}

	//�ʂ̗������鏈����Rain()�ł܂Ƃ߂čs����悤�ɂ��邽��, �����ꂽ�ʂ̂Ƃ���͋�ɂ��Ă���
	//�Ȃ�, ���ۂ̃Q�[���ɂ͋�̃}�X�͑��݂��Ȃ��̂Œ���!
	void Erase(const Point &pos)
	{
		field[pos.x][pos.y].kind = EMPTY;
	}
	void AttackOjama(const Point &pos)
	{
		if (!field[pos.x][pos.y].IsOjama()) return;
		field[pos.x][pos.y].kind++;
	}

	//�F�t���ʂ̉򂪏��������Ɋ������܂�邨����܂𐔂���
	OjamaCalculator CountOjamas(const vector<Point> &colorfulLump) const
	{
		OjamaCalculator res;
		vector< vector<int> > cntNext(W);
		for (int i = 0; i < W; i++) cntNext[i].resize(H, 0);
		res.colorfulErasure = colorfulLump.size();
		for (int i = 0; i < res.colorfulErasure; i++)
		{
			if (!field[colorfulLump[i].x][colorfulLump[i].y].IsColorful()) continue;
			for (int dir = 0; dir < 4; dir++)
			{
				Point next(colorfulLump[i]);
				next.Move(dir);
				if (IsOutside(next) || !field[next.x][next.y].IsOjama()) continue;
				cntNext[next.x][next.y]++;
				//������܂�������Ȃ�
				if (cntNext[next.x][next.y] == -field[next.x][next.y].kind)
					res.ojamaErasure++;
				//������܂���̉������Ȃ�
				if (field[next.x][next.y].IsHardOjama() && cntNext[next.x][next.y] == 1)
					res.weakness++;
			}
		}
		return res;
	}

	void Erase(const vector<Point> &lump)
	{
		for (int i = 0; i < lump.size(); i++)
		{
			Erase(lump[i]);
			for (int dir = 0; dir < 4; dir++)
			{
				Point next = lump[i];
				next.Move(dir);
				if (IsOutside(next) || !field[next.x][next.y].IsOjama()) continue;
				AttackOjama(next);
			}
		}
	}

	//�d�͂ŗ������邩�̂悤��, �ʂ����ɋl�߂�
	//��x�̏����̍�, rain[x]�ɋl�܂��Ă���ʂ�K�v�ȕ������ォ��~�点�邪,
	//  ����Ȃ����͉����~�点�����u
	//�����_���ŋʂ��~�点�ăt�B�[���h�𖄂߂Ă����Ƃ������Ƃ͂Ȃ��̂Œ���!
	void Rain()
	{
		for (int x = 0; x < W; x++)
		{
			int y = H - 2;
			for (int targetY = H - 1; targetY >= 0; targetY--)
			{
				if (!field[x][targetY].IsEmpty()) continue;
				y = min(y, targetY - 1);
				//field[x][targetY]�͋�
				while (y >= 0 && field[x][y].IsEmpty()) y--;
				//(y>=0�Ȃ�)field[x][y]�͋�łȂ�
				if (y >= 0) swap(field[x][targetY], field[x][y]);
				else if (!rain[x].empty())
				{
					field[x][targetY] = Cell(rain[x].front());
					rain[x].pop();
				}
				else break;
			}
		}
	}

	//Rain()�̌�ɌĂяo�����ׂ�
	void RandomRain()
	{
		for (int x = 0; x < W; x++)
		{
			for (int y = 0; y < H; y++)
			{
				if (field[x][y].IsEmpty()) field[x][y] = Cell(1 + Rand::Next(M));
			}
		}
	}

	//Rain()�̌�ɌĂяo�����ׂ�
	void RainHardOjama()
	{
		for (int x = 0; x < W; x++)
		{
			for (int y = 0; y < H; y++)
			{
				if (field[x][y].IsEmpty()) field[x][y] = Cell(HARD_OJAMA);
			}
		}
	}

	vector<Point> GetMin()
	{
		int mi = H*W + 1;
		vector<Point> res;
		vector< vector<bool> > used(W);
		for (int i = 0; i < W; i++) used[i].resize(H, false);
		for (int y = 0; y < H; y++)
		{
			for (int x = 0; x < W; x++)
			{
				if (!field[x][y].IsColorful() || used[x][y]) continue;
				vector<Point> ret = GetLump(Point(x, y), used);
				if (ret.size() < max(N, 4)) continue;
				if (ret.size() < mi)
				{
					mi = ret.size();
					res = ret;
				}
			}
		}
		return res;
	}

	void ReceiveOjamas(OjamaCalculator &oc)
	{
		bool isHard = oc.IsHard();
		int ojamasCount = oc.Calculate();
		for (int i = 0; i < ojamasCount; i++)
		{
			rain[Rand::Next(W)].push(isHard ? HARD_OJAMA : NORMAL_OJAMA);
		}
	}

	double Evalute(const OjamaCalculator &oc)
	{
		vector< vector<bool> > used(W);
		double eval = 0;
		for (int i = 0; i < W; i++) used[i].resize(H, false);
		{
			double ma = 0, sum = 9;
			int cnt = 0;
			State copy(*this);
			while (true)
			{
				copy.RainHardOjama();
				vector<Point> mi = copy.GetMin();
				if (mi.size() == 0) break;
				OjamaCalculator oc_ = copy.CountOjamas(mi);
				double score = oc_.Calculate()*(oc_.IsHard() ? 2 : 1)/* + oc_.ojamaErasure*0.5 + oc_.weakness*0.5*/;
				ma = max(ma, score);
				sum += score;
				copy.Erase(mi);
				copy.Rain();
				cnt++;
			}
			if (cnt > 0) eval += sum - cnt*0.5;
		}
		//if (oc.Calculate()*(oc.IsHard() ? 2 : 1) >= H*W*0.4) tmp = 1.2;
		eval += (oc.Calculate()*(oc.IsHard() ? 2 : 1)/* + oc.ojamaErasure*0.5 + oc.weakness*0.5*/)*0.7;
		return eval;
	}

	double EvaluteEnd()
	{
		//TODO
	}

	void Debug()
	{
		for (int y = 0; y < H; y++)
		{
			for (int x = 0; x < W; x++)
			{
				fprintf(stderr, "%2d ", field[x][y].kind);
			}
			fprintf(stderr, "\n");
		}
		fprintf(stderr, "\n");
		fflush(stderr);
	}
};

class States
{
public:
	int dep; //�Q�[�����[���ƈႢ�A��������U�Ŋ����U
	State state[2];
	bool isEnd[2];

	States()
	{
		dep = 0;
		isEnd[0] = isEnd[1] = false;
	}

	static States Input(int w, int h, int n, int m)
	{
		States res;
		res.state[0] = State::Input(w, h, n, m);
		res.state[1] = State::Input(w, h, n, m);
		return res;
	}

	State& GetActiveState()
	{
		return state[dep&1];
	}

	State& GetPassiveState()
	{
		return state[(dep + 1)&1];
	}

	//-1�Ȃ�Q�[�����s��
	//2�Ȃ��������
	int GetWinner() const
	{
		if (dep & 1) return -1;
		if (isEnd[0] && isEnd[1]) return 2;
		if (isEnd[0]) return 0;
		if (isEnd[1]) return 1;
		return -1;
	}

	//200�^�[���o�ߌ�̕]��
	double EvaluteEnd() const
	{
		//TODO
	}

	//�Q�[���I�[�o�[���l������
	double Evalute() const
	{
		//TODO
		//GetWinner()>=0�Ȃ炻��Ȃ�̏���������
	}

	void SendOjamas(OjamaCalculator &oc)
	{
		GetPassiveState().ReceiveOjamas(oc);
	}

	void NextTurn(const Point &p)
	{
		//TODO
	}
};

class Node
{
public:
	int gamesCount;
	double evalSum;
	vector<Node> nextNodes;
	vector<Point> nextMoves;

	Node()
	{
		evalSum = 0;
	}

	double UCB(int allGamesCount) const
	{
		//TODO
		//gamesCount==0�̂Ƃ�INF
	}

	void Visit()
	{
		//TODO
		//gamesCount�̉��Z
		//nextNodes��nextMoves�̐���
	}

	double PlayOut(States states)
	{
		//TODO
		while (true)
		{
			//���Ȃ��Ƃ��ǂ��炩���Q�[���I�[�o�[
			if (states.GetWinner() >= 0)
			{

			}
			//200�^�[�����I��
			if (states.dep > 200)
			{
				return states.EvaluteEnd();
			}

			State &activeState = states.GetActiveState();
			vector< vector<bool> > used(activeState.W);
			for (int i = 0; i < activeState.W; i++) used[i].resize(activeState.H, false);

			double ma = -DINF;
			Point res;
			State nextState;
			OjamaCalculator oc;
			for (int x = 0; x < activeState.W; x++)
			{
				for (int y = 0; y < activeState.H; y++)
				{
					if (!activeState.field[x][y].IsColorful() || used[x][y]) continue;
					vector<Point> colorfulLump = activeState.GetLump(Point(x, y), used);
					if (colorfulLump.size() < activeState.N) continue;
					OjamaCalculator _oc = activeState.CountOjamas(colorfulLump);
					State copy(activeState);
					copy.Erase(colorfulLump);
					copy.Rain();
					copy.RandomRain();
					double eval = copy.Evalute(_oc);
					if (ma < eval)
					{
						ma = eval;
						res = Point(x, y);
						nextState = copy;
						_oc = _oc;
					}
				}
			}
			states.dep++;
			states.GetActiveState() = nextState;
			states.SendOjamas(oc);
		}
	}

	double Search(States states, int allGamesCount)
	{
		int winner = states.GetWinner();
		if (winner >= 0)
		{
			return states.Evalute();
		}
		Visit();
		if (gamesCount < 30)
		{
			return PlayOut(states);
		}
		double ma = -DINF, index = -1;
		for (int i = 0; i < nextNodes.size(); i++)
		{
			int ucb = nextNodes[i].UCB(allGamesCount);
			if (ma < ucb)
			{
				ma = ucb;
				index = i;
			}
		}
		states.NextTurn(nextMoves[index]);
		double res = nextNodes[index].Search(states, allGamesCount);
		evalSum += res;
		return res;
	}
};

Point Think(const State &root)
{
	vector< vector<bool> > used(root.W);
	for (int i = 0; i < root.W; i++) used[i].resize(root.H, false);

	double ma = -DINF;
	Point res;
	for (int x = 0; x < root.W; x++)
	{
		for (int y = 0; y < root.H; y++)
		{
			if (!root.field[x][y].IsColorful() || used[x][y]) continue;
			vector<Point> colorfulLump = root.GetLump(Point(x, y), used);
			if (colorfulLump.size() < root.N) continue;
			State tmp(root);
			OjamaCalculator oc = root.CountOjamas(colorfulLump);
			tmp.Erase(colorfulLump);
			tmp.Rain();
			double sum = 0;
			for (int i = 0; i < 10; i++)
			{
				State copy(tmp);
				copy.RandomRain();
				sum += copy.Evalute(oc);
			}
			//tmp.RandomRain();
			double eval = sum / 10.0;
			if (ma < eval)
			{
				ma = eval;
				res = Point(x, y);
			}
		}
	}
	return res;
}

int myScore, rivalScore;
bool isFirst;

int main()
{
	int w, h, n, m;
	cin >> w >> h >> n >> m;
	int tmp;
	cin >> tmp;
	isFirst = (tmp == 0) ? true : false;
	cin >> myScore >> rivalScore;
	cout << "CounterAI" << endl;
	int turn = 0;
	while (true)
	{
		State myState = State::Input(w, h, n, m);
		State rivalState = State::Input(w, h, n, m);

		Point ans = Think(myState);
		cout << ans.x + 1 << " " << ans.y + 1 << endl;
		turn++;
	}
	return 0;
}