//isGameOver�𔻒肷�郁�\�b�h���Ȃ�
//

#include <iostream>
#include <cstdio>
#include <vector>
#include <queue>
#include <cstdlib>
#include <climits>
#include <cmath>
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
		return (int)(min + (double)w*(max - min) / ((double)UINT_MAX + 1));
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

const int W = 9, H = 15, N = 5, M = 3;

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
	//int W, H, N, M;
	//���W(x,y)�̏���field[x][y]�ɓ����Ă���
	//field[y][x]�ł͂Ȃ��̂Œ���!
	Cell field[W][H];
	//rain[x]�ɂ�, ��x�ɍ~��\��̋ʂ�\���������~�鏇�Ԃɋl�߂�
	queue<int> rain[W];

	State()
	{
		remainedTime /*= W = H = N = M*/ = 0;
	}

	State& operator = (const State &a)
	{
		remainedTime = a.remainedTime;
		memcpy(field, a.field, sizeof(Cell)*W*H);
		for (int i = 0; i < W; i++)
		{
			rain[i] = a.rain[i];
		}
		return *this;
	}

	State(const State &a)
	{
		*this = a;
	}

	static State Input(int w, int h, int n, int m)
	{
		State res;
		//res.W = w; res.H = h; res.N = n; res.M = m;

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
				res.field[x][y] = Cell::Input();
			}
		}
		return res;
	}

	bool IsOutside(const Point &p) const { return p.x < 0 || W <= p.x || p.y < 0 || H <= p.y; }

private:
	void getLump(vector<Point> &ret, const Point &pos, int kind, bool used[W][H])
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
	inline vector<Point> GetLump(const Point &pos, bool used[W][H]) const
	{
		vector<Point> res;
		res.reserve(15*9*0.8);
		if (used[pos.x][pos.y]) return res;
		used[pos.x][pos.y] = true;
		getLump(res, pos, field[pos.x][pos.y].kind, used);
		return res;
	}
	//used�������ō���(�g���񂹂�Ƃ��͊O���Ő錾���Ďg���񂷂ق�������?)
	inline vector<Point> GetLump(const Point &pos) const
	{
		bool used[W][H] = {};
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
	inline OjamaCalculator CountOjamas(const vector<Point> &colorfulLump) const
	{
		OjamaCalculator res;
		int cntNext[W][H] = {};
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

	//������F�t���ʂ�n��
	//������܂͏���Ɋ�������
	void Erase(const vector<Point> &colorfulLump)
	{
		for (int i = 0; i < colorfulLump.size(); i++)
		{
			Erase(colorfulLump[i]);
			for (int dir = 0; dir < 4; dir++)
			{
				Point next = colorfulLump[i];
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

	inline vector<Point> GetMin()
	{
		int mi = H*W + 1;
		vector<Point> res;
		bool used[W][H] = {};
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
		bool used[W][H] = {};
		double eval = 0;
		State copy(*this);
		copy.RainHardOjama();
		{
			double sum = 0, ma = 0;
			int cnt = 0;
			for (int x = 0; x < W; x++)
			{
				for (int y = 0; y < H; y++)
				{
					if (!field[x][y].IsColorful() || used[x][y]) continue;
					cnt++;
					vector<Point> colorfulLump = GetLump(Point(x, y), used);
					OjamaCalculator _oc = CountOjamas(colorfulLump);
					double score = _oc.Calculate()*(_oc.IsHard() ? 2 : 1) + _oc.ojamaErasure*0.5 + _oc.weakness*0.5;
					sum += score;
					ma = max(ma, score);
					copy.Erase(colorfulLump);
				}
			}
			eval += (ma*0.4 + sum*0.7)/(double)cnt;

			int remains = 0;
			for (int x = 0; x < W; x++)
			{
				for (int y = 0; y < H; y++)
				{
					if (!copy.field[x][y].IsEmpty()) remains++;
				}
			}
			eval -= remains*0.01;

			//double ma = 0, sum = 0;
			//int remains = W*H, cnt = 0;
			//State copy(*this);
			//while (true)
			//{
			//	vector<Point> mi = copy.GetMin();
			//	if (mi.size() == 0) break;
			//	OjamaCalculator oc_ = copy.CountOjamas(mi);
			//	double score = oc_.Calculate()*(oc_.IsHard() ? 2 : 1) + oc_.ojamaErasure + oc_.weakness*0.5;
			//	ma = max(ma, score);
			//	sum += score;
			//	copy.Erase(mi);
			//	copy.Rain();
			//	remains -= mi.size();
			//	cnt++;
			//}
			//if (remains > 0) eval += ma + sum - remains*0.5;

			//double ma = 0;
			//int cnt = 0;
			//for (int x = 0; x < W; x++)
			//{
			//	for (int y = 0; y < H; y++)
			//	{
			//		vector<Point> colorfulLump = GetLump(Point(x, y), used);
			//		cnt++;
			//		if (colorfulLump.size() < N) continue;
			//		OjamaCalculator _oc = CountOjamas(colorfulLump);
			//		ma = max(ma, (double)_oc.Calculate()*(_oc.IsHard() ? 2 : 1));
			//	}
			//}
			//eval += ma - cnt*0.5;

			eval += (oc.Calculate()*(oc.IsHard() ? 2 : 1) + oc.ojamaErasure*0.5 + oc.weakness*0.5)*0.7/(double)cnt;
		}
		//if (oc.Calculate()*(oc.IsHard() ? 2 : 1) >= H*W*0.4) tmp = 1.2;
		return eval;
	}

	//200�^�[���o�ߌ�̕]��
	//�������قǂ���
	int EvaluteTurnOver() const
	{
		//TODO
		int res = 0;
		for (int x = 0; x < W; x++)
		{
			queue<int> rainCopy = rain[x];
			while (!rainCopy.empty())
			{
				int tmp = rainCopy.front(); rainCopy.pop();
				if (tmp == NORMAL_OJAMA) res++;
				else if (tmp == HARD_OJAMA) res += 2;
			}
			for (int y = 0; y < H; y++)
			{
				if (field[x][y].IsNormalOjama()) res++;
				else if (field[x][y].IsHardOjama()) res += 2;
			}
		}
		if (res > 0)
		{
			fprintf(stderr, "");
		}
		return res;
	}

	//double EvaluteLeaf()
	//{
	//    return Evalute(OjamaCalculator());
	//}

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

int myScore, rivalScore;
bool isFirst;

class States
{
public:
	int dep; //�Q�[�����[���ƈႢ�A��������U�Ŋ����U
	State state[2];
	bool isGameOver[2]; //����, ����

	States()
	{
		dep = 0;
		isGameOver[0] = isGameOver[1] = false;
	}

	static States Input(bool isFirst, int w, int h, int n, int m)
	{
		States res;
		res.dep = !isFirst;
		res.state[0] = State::Input(w, h, n, m);
		res.state[1] = State::Input(w, h, n, m);
		return res;
	}

	bool IsWinjii() const
	{
		return ((dep + !isFirst)&1) == 0;
	}

	State& GetActiveState()
	{
		return state[(dep + !isFirst)&1];
	}

	State& GetPassiveState()
	{
		return state[(dep + !isFirst + 1)&1];
	}

	//-1�Ȃ�Q�[�����s��
	//2�Ȃ��������
	int GetWinner() const
	{
		if (dep & 1) return -1;
		if (isGameOver[0] && isGameOver[1]) return 2;
		if (isGameOver[0]) return 0;
		if (isGameOver[1]) return 1;
		return -1;
	}

	//�ǂ��炩���Q�[���I�[�o�[�ɂȂ����ꍇ�̕]��
	double EvaluteGameOver() const
	{
		if (!isGameOver[0] && !isGameOver[1])
		{
			fprintf(stderr, "NotFinished\n");
			return 0;
		}
		if (isGameOver[0] && isGameOver[1])
		{
			return 0;
		}
		if (isGameOver[0])
		{
			return -100;
		}
		if (isGameOver[1])
		{
			return 100;
		}
		fprintf(stderr, "!");
		return 0;
	}

	void NextMyTurn()
	{
		dep += 2;
	}

	//200�^�[���o�ߌ�̕]��
	double EvaluteTurnOver() const
	{
		int ojamaCount[2] = {state[0].EvaluteTurnOver(), state[1].EvaluteTurnOver()};
		if (ojamaCount[0] == ojamaCount[1]) return 0;
		if (ojamaCount[0] > ojamaCount[1]) return -100;
		if (ojamaCount[0] < ojamaCount[1]) return 100;
		fprintf(stderr, "!");
		return 0;
	}

	double EvaluteLeaf()
	{
		//�蔲��
		int ojamaCount[2] = {state[0].EvaluteTurnOver(), state[1].EvaluteTurnOver()};
		double eval[2] = {state[0].Evalute(OjamaCalculator()), state[1].Evalute(OjamaCalculator())};
		return (double)(eval[0]*5 + ojamaCount[1] - eval[1]*5 - ojamaCount[0]);
	}

	void SendOjamas(OjamaCalculator &oc)
	{
		GetPassiveState().ReceiveOjamas(oc);
	}

	void NextTurn(const vector<Point> &colorfulLump)
	{
		OjamaCalculator oc = GetActiveState().CountOjamas(colorfulLump);
		GetActiveState().Erase(colorfulLump);
		GetActiveState().Rain();
		GetActiveState().RandomRain();
		SendOjamas(oc);
		dep++;
	}

	void NextTurn(const Point &p)
	{
		vector<Point> colorfulLump = GetActiveState().GetLump(p);
		NextTurn(colorfulLump);
	}
};

States currentStates;

class Node
{
public:
	int gamesCount;
	double evalSum;
	vector< vector<Node> > nextNodes;

	Node()
	{
		gamesCount = 0;
		evalSum = 0;
	}

	double UCB(bool willReverse, int allGamesCount) const
	{
		//TODO ����
		//gamesCount==0�̂Ƃ�INF
		if (gamesCount == 0) return DINF;
		//�����ɂ���Ă͊��Ғl���Ђ�����Ԃ�
		int r = willReverse ? -1 : 1;
		return evalSum*r/(double)gamesCount + 100*sqrt(log(allGamesCount)/(double)gamesCount);
	}

	void Develop(const State &activeState)
	{
		nextNodes.resize(/*activeState.*/W);
		for (int x = 0; x < /*activeState.*/W; x++) nextNodes[x].resize(/*activeState.*/H);
	}

	void Visit(const State &activeState)
	{
#pragma omp atomic
		gamesCount++;
		//���߂Ă������l�𒴂�����W�J
		if (gamesCount == 10)
		{
			Develop(activeState);
		}
	}

	double PlayOut(States states)
	{
		while (true)
		{
			//���Ȃ��Ƃ��ǂ��炩���Q�[���I�[�o�[
			if (states.GetWinner() >= 0)
			{
				return states.EvaluteGameOver();
			}
			//200�^�[�����I��
			if (states.dep >= 400)
			{
				return states.EvaluteTurnOver();
			}
			//�[�����E
			if (states.dep - currentStates.dep > 16)
			{
				double res = states.EvaluteLeaf();
				if (abs(res) > 1e-8)
				{
					fprintf(stderr, "");
				}
				return res;
			}

			State &activeState = states.GetActiveState();
			bool used[W][H] = {};

			double ma = -DINF;
			Point res;
			State nextState;
			OjamaCalculator oc;
			states.isGameOver[!states.IsWinjii()] = true;
			for (int x = 0; x < /*activeState.*/W; x++)
			{
				for (int y = 0; y < /*activeState.*/H; y++)
				{
					if (!activeState.field[x][y].IsColorful() || used[x][y]) continue;
					vector<Point> colorfulLump = activeState.GetLump(Point(x, y), used);
					if (colorfulLump.size() < /*activeState.*/N) continue;
					states.isGameOver[!states.IsWinjii()] = false;
					OjamaCalculator _oc = activeState.CountOjamas(colorfulLump);
					State copy(activeState);
					copy.Erase(colorfulLump);
					copy.Rain();
					double eval = copy.Evalute(_oc);
					if (ma < eval)
					{
						ma = eval;
						res = Point(x, y);
						nextState = copy;
						oc = _oc;
					}
				}
			}
			if (states.isGameOver[!states.IsWinjii()])
			{
				states.dep++;
				continue;
			}
			nextState.RandomRain();
			states.GetActiveState() = nextState;
			states.SendOjamas(oc);
			if (!states.IsWinjii()) states.SendOjamas(oc);
			states.dep++;
		}
	}

	double Search(States &states, int allGamesCount)
	{
		//for (int i = 0; i <= states.dep; i++) fprintf(stderr, " ");
		//fprintf(stderr, "dep = %d\n", states.dep);
		//200�^�[�����o��
		if (states.dep >= 400)
		{
			double res = states.EvaluteTurnOver();
			evalSum += res;
			return res;
		}
		//�ǂ��炩���Q�[���I�[�o�[
		if (states.GetWinner() >= 0)
		{
			double res = states.EvaluteGameOver();
			evalSum += res;
			return res;
		}
		State &activeState = states.GetActiveState();
		Visit(activeState);
		//�������l�𒴂��ĂȂ��̂Ńv���C�A�E�g��
		if (nextNodes.size() == 0)
		{
			//for (int i = 0; i <= states.dep; i++) fprintf(stderr, " ");
			//fprintf(stderr, "PlayOut\n");
			double res = PlayOut(states);
			evalSum += res;
			if (abs(res) > 1e-8)
			{
				;
			}
			return res;
		}
		double ma = -DINF;
		Point nextMove;
		bool used[W][H] = {};

		for (int x = 0; x < nextNodes.size(); x++)
		{
			for (int y = 0; y < nextNodes[x].size(); y++)
			{
				if (!activeState.field[x][y].IsColorful() || used[x][y]) continue;
				vector<Point> colorfulLump = activeState.GetLump(Point(x, y), used);
				if (colorfulLump.size() < /*activeState.*/N) continue;
				double ucb = nextNodes[x][y].UCB(!states.IsWinjii(), allGamesCount);
				if (ma < ucb)
				{
					ma = ucb;
					nextMove = Point(x, y);
				}
			}
		}
		if (nextMove == Point())
		{
			evalSum += 0;
			return 0;
		}
		states.NextTurn(nextMove);
		double res = nextNodes[nextMove.x][nextMove.y].Search(states, allGamesCount);
		evalSum += res;
		//for (int i = 0; i <= states.dep; i++) fprintf(stderr, " ");
		//fprintf(stderr, "Update\n");
		return res;
	}
};

Point Think(States &firstStates)
{
	int allGamesCount = 0;
	Node root;
	root.Develop(firstStates.GetActiveState());
#pragma omp parallel for
	for (int i = 0; i < 1000; i++)
	{
		//fprintf(stderr, "allGamesCount == %d\n", i);
		States states(firstStates);
		root.Search(states, allGamesCount);
#pragma omp atomic
		allGamesCount++;
	}

	for (int x = 0; x < 9; x++)
	{
		for (int y = 0; y < 15; y++)
		{
			fprintf(stderr, "(%d,%d), %d/%d, %f,  %f\n", x, y, root.nextNodes[x][y].gamesCount, allGamesCount, root.nextNodes[x][y].evalSum, root.nextNodes[x][y].UCB(false, 200));
		}
	}

	Point res;
	int ma = 0;
	for (int x = 0; x < /*firstStates.GetActiveState().*/W; x++)
	{
		for (int y = 0; y < /*firstStates.GetActiveState().*/H; y++)
		{
			if (ma < root.nextNodes[x][y].gamesCount)
			{
				ma = root.nextNodes[x][y].gamesCount;
				res = Point(x, y);
			}
		}
	}
	fprintf(stderr, "%d\n", ma);
	fflush(stderr);
	return res;
}

int main()
{
	int w, h, n, m;
	cin >> w >> h >> n >> m;
	int tmp;
	cin >> tmp;
	isFirst = (tmp == 0) ? true : false;
	cin >> myScore >> rivalScore;
	cout << "Winjii" << endl;
	int turn = 0;
	while (true)
	{
		currentStates = States::Input(isFirst, w, h, n, m);

		Point ans = Think(currentStates);
		cout << ans.x + 1 << " " << ans.y + 1 << endl;
		turn++;
	}
	return 0;
}
