# include <Windows.h>
# include <string>



void FillRect(HDC hDC, const RECT* pRect, COLORREF color)
{
	COLORREF oldColor = SetBkColor(hDC, color);
	ExtTextOut(hDC, 0, 0, ETO_OPAQUE, pRect, TEXT(""), 0, 0);
	SetBkColor(hDC, oldColor);
}

// Use this to draw text if you need to.
void DrawTextLine(HWND hWnd, HDC hDC, const char* sText, RECT* pr_topext, COLORREF clr);


namespace game
{
	unsigned int windowWidth = 0;
	unsigned int windowHeight = 0;
	char direction = ' ';
	int x_pos, y_pos ;
	int x_apple, y_apple;
	bool active = 1;
	int AvalibalePosition[] = {0, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 360, 390, 420, 450, 480, 510, 540, 570};//avaliable position for apple 
	
	struct Pos
	{
		int x;
		int y;
		Pos( int _x = 0, int _y = 0) { x = _x; y = _y; }
	};


    int node_count = 0;
	struct node
	{
		RECT rec;
		node* next;
		node* prev;
		node() { node_count++; }
		~node() { node_count--; }
	};
	node* head;
	node* tail;

	void BuildSnake()
	{
		int initialize_x = 30;
		int initialize_y = 300;

		head = tail =NULL;
		for ( int i=0; i<4 ;i++)
		{
			RECT r;
			r.left = initialize_x;
			r.top = initialize_y;
			r.right = r.left + 20;
			r.bottom = r.top + 20;
			initialize_x+= 30;

			node* tmp = new node;
			tmp -> rec = r;
			tmp -> next = NULL;

			if(head == NULL)
				head = tail =tmp;
			
			else
			{
				head -> next = tmp;
				head = tmp;
			}
		}
		x_pos = (head->rec).left;
		y_pos = (head->rec).top;
	}

	bool Initialize(HWND hWnd)
	{

		HDC hDC = GetDC(hWnd);
		RECT rClient;
		GetClientRect(hWnd, &rClient);
		windowWidth = rClient.right - rClient.left; // rClient.top and rClient.left are always 0.
		windowHeight = rClient.bottom - rClient.top;

		active = false;
		direction=' ';
		BuildSnake();

		//apple
		x_apple = 240;
		y_apple = 210;
		
		OutputDebugStringA("My game has been initialized. This text should be shown in the 'output' window in VS");	
		
		return true;
	}


	// Samar: please review 'passing by reference vs passing by value'
	Pos get_next_position(char direction, Pos headPos)
	{
		Pos newPos = headPos;
		if(direction == 'd')//Left direction
			newPos.x -= 30;
		else if(direction == 's')//down direction
			newPos.y += 30;
		else if(direction == 'a')//right direction
			newPos.x += 30;
		else if(direction == 'w')//top direction
			newPos.y -= 30;
		return newPos;
	}


	void draw_apple(HWND hWnd)
	{
		HDC hDC = GetDC(hWnd);
		
		RECT r_apple;
		r_apple.left = x_apple;
		r_apple.right = r_apple.left + 20;
		r_apple.top = y_apple;
		r_apple.bottom = r_apple.top + 20;
		FillRect(hDC, &(r_apple), RGB(238,130,238));
	}

	bool  EatenApple()
	{
		if((head->rec).left == x_apple && (head->rec).top == y_apple)
			return true;
		return false;
	}

	void NewHeadNode(int x, int y)
	{
		RECT r;
		r.left = x;
		r.right = x + 20;
		r.top = y;
		r.bottom = y+ 20;
		
		node* tmp = new node;
		tmp ->rec = r;
		tmp ->next = NULL;
		head ->next = tmp;
		head = tmp;
	}

	bool hit_wall()
	{
		if (direction == 'a')
		{
			if( (head->rec).right >= windowWidth )
				return true;
		}
		else if (direction == 'd') 
		{
			if( (head->rec).left == 0)
				return true;
		}
		else if (direction == 'w') 
		{
			if( (head->rec).top == 0)
				return true;
		}
		else if (direction == 's') 
		{
			if( (head->rec).bottom >= windowHeight)
				return true;
		}

		return false;
	}

	bool eat_itself()
	{
	    node* t = tail;
		while ( t != head )
		{
			if( (head->rec).left == (t->rec).left  && (head->rec).top == (t->rec).top)
				return true;
			
				t = t->next;
		}
		return false;
	}


	void DrawSnake( HDC hDC )
	{
		node* t = tail;
		while(t != head)
		{
			FillRect(hDC, &(t->rec), RGB(255, 255, 0));
			t = t->next;
		}
		FillRect(hDC, &(head->rec), RGB(0,255,0));
	}


	// This is called periodically. Use it to update your game state and draw to the window.
	void CALLBACK OnTimer(HWND hWnd, UINT Msg, UINT_PTR idTimer, DWORD dwTime)
	{
		HDC hDC = GetDC(hWnd);
		RECT rClient;
		GetClientRect(hWnd, &rClient);
		FillRect(hDC, &rClient, RGB(0,0,128)); // Clear the window to blackness.
		char text[256] = { 0 };
		//sprintf_s(text, "OnTimer. Time: %d %d %d %d %d %d",(head->rec).left ,(head->rec).top ,(tail->rec).left ,(tail->rec).top ,windowHeight ,windowWidth);
		//sprintf_s(text, "Node count: %d", sizeof(AvalibalePosition)/sizeof(int));
		sprintf_s(text, "Node count: %d", node_count);
		RECT r_topext = { 0, 0, rClient.right, 30 };
		DrawTextLine(hWnd, hDC, text, &r_topext, RGB(120, 120, 120));
	
		draw_apple(hWnd);

		DrawSnake( hDC );

		if( !active )
			return;

	    // check if snake eat itself
		if( eat_itself() )
		{
			active = false;
			MessageBox(hWnd, TEXT("YOU LOST!, EatItself"), TEXT("Snake"), MB_OK );
			Initialize(hWnd);
		}
			
		// check if snake hit the wall
		else if( hit_wall() )
		{
			active = false;
			MessageBox(hWnd, TEXT("YOU LOST!!!,HitWall"), TEXT("Snake"), MB_OK );
			Initialize(hWnd);
		}	

		else if( active )
		{
			Pos headPos; headPos.x = x_pos; headPos.y = y_pos;
			Pos newPos = get_next_position(direction, headPos);
			NewHeadNode(newPos.x, newPos.y);
			x_pos = newPos.x; y_pos = newPos.y;
		
			node* p = tail;
			tail = tail->next;
			delete(p);
		
			if(EatenApple())
			{
				Pos newPos = get_next_position( direction, Pos(x_pos, y_pos));
				NewHeadNode(newPos.x, newPos.y);
				x_pos = newPos.x; y_pos = newPos.y;
			
				x_apple = AvalibalePosition[rand() % 15];
				y_apple = AvalibalePosition[rand() % 15];
				bool IsBody = 1;
				while(IsBody)
				{
					IsBody = false;
					node* t = tail;
					while ( t != head )
					{
						if( (t->rec).left == x_apple  && (t->rec).top == y_apple)
						{
							IsBody = true;
							break;
						}
						else
							t = t->next;
					}
					if(IsBody){
						x_apple = AvalibalePosition[rand() % 15];
						y_apple = AvalibalePosition[rand() % 15];
					}

				}

			}
		}
	}
	// This is called when the user presses a key on the keyboard.
	// Use this to change the direction of your snake.
	bool OnKeyDown(WPARAM keyCode)
	{
		if(keyCode == VK_UP && direction != 's')
			direction = 'w';
		else if(keyCode == VK_DOWN && direction != 'w')
			direction = 's';
		else if(keyCode == VK_LEFT && direction != 'a')
			direction = 'd';
		else if(keyCode == VK_RIGHT && direction != 'd')
			direction = 'a';

		if (keyCode == VK_UP || keyCode == VK_DOWN || keyCode == VK_LEFT || keyCode == VK_RIGHT){
			active = true;
			return true;
		}
		return false; // They key pressed does not interest us. Let the OS handle it.
	}
		
} 
