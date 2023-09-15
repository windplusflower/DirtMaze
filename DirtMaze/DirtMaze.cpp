#include <Windows.h>
#include<bits/stdc++.h>
using std::vector;
using std::pair;
using std::ofstream;
const int maxn=1e6;
vector<pair<double,double>>spike,lever,door;//物品坐标
vector<pair<double,double>> generate_spike(int seed);//生成刺
vector<pair<double,double>> generate_lever(int n,int seed);//生成开关
vector<pair<double,double>> generate_door(int n);//生成门
//各种输出，因为找不到好用的json库所以用了蹩脚的字符串输出
void print_init(ofstream &fout,int seed,int levernum);
void print_spike(ofstream &fout,vector<pair<double,double>>&spike);
void print_lever(ofstream &fout,vector<pair<double,double>>&spike);
void print_door(ofstream &fout,vector<pair<double,double>>&spike);
void print_end(ofstream &fout);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//窗口程序
HINSTANCE hInst;
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR szCmdLine,int iCmdShow){
	static TCHAR szClassName[] = TEXT("HelloWin");  
	HWND hwnd;         //窗口句柄
	MSG msg;           //消息
	WNDCLASS wndclass; // 窗口类
	hInst = hInstance;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;	
	wndclass.lpfnWndProc = WndProc;		
	wndclass.cbClsExtra = 0;			
	wndclass.cbWndExtra = 0;			
	wndclass.hInstance = hInstance;			
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	
	wndclass.hCursor = LoadIcon(NULL, IDC_HAND);	
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;		
	wndclass.lpszClassName = szClassName; 
	RegisterClass(&wndclass);
	hwnd = CreateWindow(
		szClassName,		
		TEXT("DirtMaze"),	
		WS_OVERLAPPEDWINDOW,	
		CW_USEDEFAULT,		
		CW_USEDEFAULT,			
		380,			
		250,		
		NULL,		
		NULL,			
		hInstance,		
		NULL			
	);
	//显示窗口
	ShowWindow(hwnd, iCmdShow);
	//更新窗口
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
//第一次写GUI，啥也不会，于是网上随便找了段代码改了改
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;		  //设备环境句柄
	PAINTSTRUCT ps;   //存储绘图环境的相关信息
	int wmId, wmEvent;
	int seed,levernum;
	static HFONT hFont;  //字体
	static HWND tip1;  //种子描述
	static HWND tip2;  //开关数描述
	static HWND text_seed;  //输入种子数
	static HWND text_lever;  //输入开关框
	static HWND button_seed;  //根据种子生成
	static HWND button_random;  //随机生成
	static HWND maker; 
	//定义缓冲区
	TCHAR szseed[100];
	TCHAR szlever[100];
	TCHAR szUserInfo[200];
	ofstream fout;
	switch (message)
	{
	case WM_CREATE:		
		//创建逻辑字体
		hFont = CreateFont(
			-14, -7, 0, 0, 400 ,
			FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
			FF_DONTCARE, TEXT("微软雅黑")
		);

		tip1 = CreateWindow(
			TEXT("static"), TEXT("种子："),
			WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT,
			0 , 20 , 70 , 26 ,
			hWnd, (HMENU)1, hInst, NULL
		);
		tip2 = CreateWindow(TEXT("static"), TEXT("开关数量："),
			WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_RIGHT ,
			0, 56, 70, 26,
			hWnd, (HMENU)2, hInst, NULL
		);
		text_seed = CreateWindow(TEXT("edit"), TEXT("0"),
			WS_CHILD | WS_VISIBLE | WS_BORDER| ES_AUTOHSCROLL,
			80, 20, 200, 26,
			hWnd, (HMENU)3, hInst, NULL
		);
		text_lever = CreateWindow(TEXT("edit"), TEXT("0"),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL ,
			80, 56, 200, 26,
			hWnd, (HMENU)4, hInst, NULL
		);
		//创建按钮控件
		button_seed = CreateWindow(TEXT("button"), TEXT("按种子生成"),
			WS_CHILD | WS_VISIBLE | WS_BORDER | BS_FLAT,
			80, 92, 200, 30,
			hWnd, (HMENU)5, hInst, NULL
		);
		button_random = CreateWindow(TEXT("button"), TEXT("随机生成"),
			WS_CHILD | WS_VISIBLE | WS_BORDER | BS_FLAT,
			80, 128, 200, 30,
			hWnd, (HMENU)6, hInst, NULL
		);
		maker = CreateWindow(TEXT("static"), TEXT("Made By Wind-_-Flower"),
			WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE| SS_CENTER ,
			80, 164, 200, 26,
			hWnd, (HMENU)7, hInst, NULL
		);
		//依次设置控件的字体
		SendMessage(tip1, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(tip2, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(text_seed, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(text_lever, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(button_seed, WM_SETFONT, (WPARAM)hFont, NULL);
		SendMessage(button_random, WM_SETFONT, (WPARAM)hFont, NULL);
		break;
	case WM_COMMAND://按钮相应
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		if(wmId!=5&&wmId!=6) return DefWindowProc(hWnd, message, wParam, lParam);
		GetWindowText(text_seed, szseed, 100);
		GetWindowText(text_lever, szlever, 100);
		seed=0;
		for(int i=0;szseed[i]!='\0';i++)seed=seed*10+szseed[i]-'0';
		levernum=0;
		for(int i=0;szlever[i]!='\0';i++)levernum=levernum*10+szlever[i]-'0';
		levernum=std::min(levernum,38);
		if (wmId==6)seed=-1;
		spike=generate_spike(seed);
		lever=generate_lever(levernum,seed);
		door=generate_door(levernum);
		fout.open("Town.json");
		if(!fout.is_open()){
			MessageBox(hWnd, "文件生成失败！", TEXT("信息提示"), MB_ICONINFORMATION);
			break;
		}
		print_init(fout,seed,levernum);
		print_spike(fout,spike);
		print_lever(fout,lever);
		print_door(fout,door);
		print_end(fout);
		fout.close();
		MessageBox(hWnd, "文件生成成功！", TEXT("信息提示"), MB_ICONINFORMATION);
		break;
	case WM_DESTROY:		//窗口销毁消息
		DeleteObject(hFont);	//删除创建的字体
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
vector<pair<double,double>> generate_spike(int seed){
	vector<pair<double,double>>spike;
	vector<pair<int,int>>edge,block;//edge表示两点有边，block表示两点间有刺
	if(seed==-1)seed=time(0);
	srand(seed);
	//并查集
	vector<int>fa(maxn,0);
	std::function<int(int)>find=[&](int x){
		return fa[x]=fa[x]==x?x:find(fa[x]);
	};
	//二维序号与一维序号的映射，为了方便并查集
	auto hash=[](int x,int y){
		return x*1000+y;
	};
	auto unhash=[](int a)->pair<int,int>{
		return {a/1000,a%1000};
	};
	for(int i=1;i<maxn;i++)fa[i]=i;
	for(int i=3;i<=40;i++)
		for(int j=2;j<=11;j++){
			if(i>=11&&i<28&&j<5){
				//德特茅斯这块区域视角有问题会看不到小骑士，干脆就不让小骑士路过这里
				if(i==11||i==27)block.push_back({hash(i,j),hash(i+1,j)});
				if(j==4)block.push_back({hash(i,j),hash(i,j+1)});
				continue;
			}
			edge.push_back({hash(i,j),hash(i+1,j)});
			edge.push_back({hash(i,j),hash(i,j+1)});
		}
	random_shuffle(edge.begin(),edge.end());
	//外围和内围框起来
	for(int i=3;i<=40;i++){
		block.push_back({hash(i,11),hash(i,12)});
		block.push_back({hash(i,1),hash(i,2)});
	}
	for(int j=2;j<=11;j++){
		block.push_back({hash(2,j),hash(3,j)});
		block.push_back({hash(40,j),hash(41,j)});
	}
	//随机生成路径
	for(auto v:edge){
		int x=find(v.first),y=find(v.second);
		if(x!=y) fa[x]=y;
		else block.push_back({v.first,v.second});
	}
	for(auto v:block){
		pair<int,int>a=unhash(v.first),b=unhash(v.second);
		//入口和出口不放电锯
		if((a.first==3||b.first==3)&&a.second==8&&b.second==8)continue;
		if(a.first==31&&b.first==31&&(a.second==2||b.second==2))continue;
		int dx=b.first-a.first,dy=b.second-a.second;
		double x=(a.first+b.first)*3.0,y=(a.second+b.second)*3.0;
		for(int i=-1;i<=1;i++)spike.push_back({x+i*dy*2,y+i*dx*2});
	}
	return spike;
}
vector<pair<double,double>> generate_lever(int n,int seed){
	vector<pair<double,double>>lever;
	srand(seed);
	//把地图分成n份，每份放一个开关，避免开关生成在一块
	for(int i=1;i<=n;i++){
		int x=rand()%(38/n)+3+38/n*(i-1);
		int y=rand()%10+2;
		if(x>=11&&x<28&&y<5){
			i--;
			continue;
		}
		lever.push_back({x*6,y*6});
	}
	return lever;
}
vector<pair<double,double>> generate_door(int n){
	vector<pair<double,double>>door;
	//门之间加点便宜好看点（应该吧
	while(n--){
		door.push_back({184.897232,9.2-0.2*n});
	}
	return door;
}
//蹩脚的输出
void print_init(ofstream &fout,int seed,int levernum){
	//这部分是椅子、存档点、望远镜、石碑等与随机无关的物品
	fout<<R"({
  "$type": "DecorationMaster.ItemSettings, DecorationMaster",
  "scene_name": "Town",
  "mod_version": 0.5,
  "items": {
    "$type": "System.Collections.Generic.List`1[[DecorationMaster.Item, DecorationMaster]], mscorlib",
    "$values": [
      {
        "$type": "DecorationMaster.MyBehaviour.Bench+BenchItem, DecorationMaster",
        "bench_name": "D_bench1_667844_94532266",
        "sceneName": "Town",
        "pname": "HK_bench",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 8.466211,
          "y": 43.73064
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+LoreTablet1, DecorationMaster",)"<<std::endl;
	fout<<"        \"Text\": \"attack to active the observer\\njump to close the observer\\nseed: "<<seed<<"\\nnumber of lever:"<<levernum<<"\",\n";
	fout<<R"(        "size": 0.685402632,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_lore_tablet_1",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 5.886715,
          "y": 44.5104866
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 11.4206257,
          "y": 44.0605736
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_rect",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.3311,
          "y": 8.1907835
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_rect",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 188.495941,
          "y": 8.113518
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_small",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.90393,
          "y": 63.4198151
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.88172,
          "y": 64.52453
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.9133759,
          "y": 65.07111
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_small",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.78089,
          "y": 51.5411644
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_small",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.78293,
          "y": 39.8582
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.6981,
          "y": 53.1984329
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.770134,
          "y": 41.5694923
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.7865,
          "y": 52.8012123
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.78788,
          "y": 41.01319
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_small",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.7495651,
          "y": 27.5623589
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.56438,
          "y": 29.1694221
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 94.73313,
          "y": 28.6824322
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_small",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.282639,
          "y": 9.937373
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_small",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.277267,
          "y": 21.9249439
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_small",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.275787,
          "y": 33.6803436
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_small",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.27066,
          "y": 45.53198
        }
      },
      {
        "$type": "DecorationMaster.MyBehaviour.DefaultBehaviour+SharedItem, DecorationMaster",
        "size": 1.0,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_platform_small",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.291489,
          "y": 57.73645
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.2828,
          "y": 59.42545
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.284027,
          "y": 47.44316
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.2828,
          "y": 35.41019
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.28273,
          "y": 23.6032734
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 180.795792,
          "y": 11.6174774
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.23671,
          "y": 10.7930489
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.241562,
          "y": 34.6103935
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.221848,
          "y": 47.3034058
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.225647,
          "y": 58.8024063
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "binoculars",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 181.297745,
          "y": 23.1161785
        }
      },
      {
        "$type": "DecorationMaster.ItemDef+DefaultItem, DecorationMaster",
        "sceneName": "Town",
        "pname": "IMG_RespawnPoint",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",
          "x": 11.2352238,
          "y": 44.2367668
        }
      })"<<std::endl;
}
void print_spike(ofstream &fout,vector<pair<double,double>>&spike){
	for(auto v:spike){
    	fout<<R"(,
      {
        "$type": "DecorationMaster.ItemDef+DefatulResizeItem, DecorationMaster",
        "size": 1.3,
        "angle": 0,
        "sceneName": "Town",
        "pname": "white_spike",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",)"<<std::endl;
		fout<<"          \"x\": "<<v.first<<","<<std::endl;
		fout<<"          \"y\": "<<v.second<<std::endl;
        fout<<R"(}
      })"<<std::endl;
	}
}
void print_lever(ofstream &fout,vector<pair<double,double>>&lever){
	int cnt=1;//开关的编号，与门对应
	for(auto v:lever){
		fout<<R"(,
      {
        "$type": "DecorationMaster.ItemDef+LeverGateItem, DecorationMaster",)"<<std::endl;
		fout<<"        \"Number\": "<<cnt++<<",\n";
		fout<<R"(        "size": 1.5,
        "angle": 0,
        "sceneName": "Town",
        "pname": "HK_lever",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",)"<<std::endl;
		fout<<"          \"x\": "<<v.first<<","<<std::endl;
		fout<<"          \"y\": "<<v.second<<std::endl;
		fout<<R"(        }
      })"<<std::endl;
	}
}
void print_door(ofstream &fout,vector<pair<double,double>>&door){
	int cnt=1;//门的编号，与开关对应
	for(auto v:door){
		fout<<R"(,
      {
        "$type": "DecorationMaster.ItemDef+LeverGateItem, DecorationMaster",)"<<std::endl;
		fout<<"        \"Number\": "<<cnt++<<",\n";
		fout<<R"(        "size": 1.0,
        "angle": 90,
        "sceneName": "Town",
        "pname": "HK_gate",
        "position": {
          "$type": "DecorationMaster.V2, DecorationMaster",)"<<std::endl;
		fout<<"          \"x\": "<<v.first<<","<<std::endl;
		fout<<"          \"y\": "<<v.second<<std::endl;
		fout<<R"(        }
      })"<<std::endl;
	}
}
void print_end(ofstream &fout){
	fout<<R"(    ]
  }
})"<<std::endl;
}
