function ShowDlg(_meindex, _talkerindex, _page,_transdata,_seqno)
	local _maxpage = math.ceil(table.getn(_transdata) / 7);
	local token = "1 绢蠢 镑栏肺 捞悼窍矫摆嚼聪鳖?？\n";
	local topselect = (_page-_seqno) * 7;
	for i = 1, 7 do
		if (i + topselect) > table.getn(_transdata) then
			break;
		end
		if _transdata[i + topselect][6] ~= nil then
			token = token .. string.format("  %-18s%s\n", _transdata[i + topselect][1], _transdata[i + topselect][6]);
		else
			token = token .. string.format("  %-18s%s\n", _transdata[i + topselect][1], map.getFloorName(_transdata[i + topselect][2]));
		end
	end
	local mypage = _page - _seqno + 1;
	if _maxpage == 1 then
		button = 8;
	elseif mypage == 1 and mypage < _maxpage then
		button = 8+32
	elseif mypage > 1 and mypage < _maxpage then
		button = 8+16+32;
	elseif mypage == _maxpage then
		button = 8+16;
	end
	lssproto.windows(_talkerindex, 2, button, _page, char.getWorkInt( _meindex, "对象"), token);
end


function FreeTieXin(charaindex)
	local token = "2\n请问有什么需要我帮忙的么？\n\n"
				.."                  [style c=5]〖 荤成磐 〗[/style]\n"
				.."                  [style c=5]〖 任务传送 〗[/style]\n"
				.."                  [style c=5]〖 庄园传送 〗[/style]\n"
				.."                  [style c=5]〖 抓宠传送 〗[/style]\n"
				.."                  [style c=5]〖 付阑捞悼 〗[/style]";
	lssproto.windows(charaindex, 2, 8, 0, char.getWorkInt( npcindex, "对象"), token);
end

--NPC对话事件(NPC索引)
function Talked(_meindex, _talkerindex, _szMes, _color )
	local token = "2\n请问有什么需要我帮忙的么？\n\n"
				.."                  [style c=5]〖 荤成磐 〗[/style]\n"
				.."                  [style c=5]〖 任务传送 〗[/style]\n"
				.."                  [style c=5]〖 庄园传送 〗[/style]\n"
				.."                  [style c=5]〖 抓宠传送 〗[/style]\n"
				.."                  [style c=5]〖 付阑捞悼 〗[/style]";
	lssproto.windows(_talkerindex, 2, 8, 0, char.getWorkInt( _meindex, "对象"), token);
end

function ShowPetType(_meindex, _talkerindex, _page)
	local _maxpage = math.ceil(table.getn(PetList) / 7);
	local topselect = (_page-10) * 7;

	local token = "1 请选择你要抓的宠物类型:\n";
	for i = 1, 7 do
		if (i + topselect) > table.getn(PetList) then
			break;
		end
		token = token .. string.format("  %s\n", PetList[i + topselect][1]);
	end
			
	local mypage = _page - 10 + 1;
	if _maxpage == 1 then
		button = 8;
	elseif mypage == 1 and mypage < _maxpage then
		button = 8+32
	elseif mypage > 1 and mypage < _maxpage then
		button = 8+16+32;
	elseif mypage == _maxpage then
		button = 8+16;
	end
	lssproto.windows(_talkerindex, 2, button, _page, char.getWorkInt( _meindex, "对象"), token);
end

--NPC窗口事件(NPC索引)
function WindowTalked (_meindex, _talkerindex, _seqno, _select, _data)
	--print(_meindex, _talkerindex, _seqno, _select, _data)
	if _seqno == 0 then
		local setid = other.atoi(_data);
		if setid == 1 then
			ShowDlg(_meindex, _talkerindex, 100,LianJiList,100);
		elseif setid == 2 then
			ShowDlg(_meindex, _talkerindex, 400,RenWuList,400);
		elseif setid == 3 then
			ShowDlg(_meindex, _talkerindex, 300,ZhuanYList,300);
		elseif setid == 4 then
			ShowPetType(_meindex, _talkerindex, 10);
		elseif setid == 5 then
			ShowDlg(_meindex, _talkerindex, 200,ChengList,200);
		end
	elseif _seqno >= 10 and _seqno < 100 then
		if _select == 0 then
			local num = other.atoi(_data) + ((_seqno - 10) * 7);
			if num > table.getn(PetList) then--防止溢出
				return ;
			end
			ShowDlg(_meindex, _talkerindex, 800,PetList[num][2],800);
			char.setWorkInt(_talkerindex,"临时整数1",num);
		elseif _select == 16 then
			ShowPetType(_meindex, _talkerindex, _seqno - 1);
		elseif _select == 32 then
			ShowPetType(_meindex, _talkerindex, _seqno + 1);
		end
	elseif _seqno >= 100 and _seqno < 200 then
		if _select == 0 then
			local num = other.atoi(_data) + ((_seqno - 100) * 7);
			if num > table.getn(LianJiList) then--防止溢出
				return ;
			end
			if LianJiList[num][5] == false then
				if char.getWorkInt(_talkerindex,"组队") > 0 then
					char.TalkToCli(_talkerindex, _meindex, "不可组队传送！", 6);
					return;
				end
			end
			char.AllWarpToSpecificPoint(_talkerindex, LianJiList[num][2], LianJiList[num][3], LianJiList[num][4]);
		elseif _select == 16 then
			ShowDlg(_meindex, _talkerindex, _seqno - 1, LianJiList,100);
		elseif _select == 32 then
			ShowDlg(_meindex, _talkerindex, _seqno + 1, LianJiList,100);
		end
	elseif _seqno >= 200 and _seqno < 300 then
		if _select == 0 then
			local num = other.atoi(_data) + ((_seqno - 200) * 7);
			if num > table.getn(ChengList) then--防止溢出
				return ;
			end
			if ChengList[num][5] == false then
				if char.getWorkInt(_talkerindex,"组队") > 0 then
					char.TalkToCli(_talkerindex, _meindex, "不可组队传送！", 6);
					return;
				end
			end
			char.AllWarpToSpecificPoint(_talkerindex, ChengList[num][2], ChengList[num][3], ChengList[num][4]);
		elseif _select == 16 then
			ShowDlg(_meindex, _talkerindex, _seqno - 1, ChengList,200);
		elseif _select == 32 then
			ShowDlg(_meindex, _talkerindex, _seqno + 1, ChengList,200);
		end
	elseif _seqno >= 300 and _seqno < 400 then
		if _select == 0 then
			local num = other.atoi(_data) + ((_seqno - 300) * 7);
			if num > table.getn(ZhuanYList) then--防止溢出
				return ;
			end
			if ZhuanYList[num][5] == false then
				if char.getWorkInt(_talkerindex,"组队") > 0 then
					char.TalkToCli(_talkerindex, _meindex, "不可组队传送！", 6);
					return;
				end
			end
			char.AllWarpToSpecificPoint(_talkerindex, ZhuanYList[num][2], ZhuanYList[num][3], ZhuanYList[num][4]);
		elseif _select == 16 then
			ShowDlg(_meindex, _talkerindex, _seqno - 1, ZhuanYList,300);
		elseif _select == 32 then
			ShowDlg(_meindex, _talkerindex, _seqno + 1, ZhuanYList,300);
		end
	elseif _seqno >= 400 and _seqno < 500 then
		if _select == 0 then
			local num = other.atoi(_data) + ((_seqno - 400) * 7);
			if num > table.getn(RenWuList) then--防止溢出
				return ;
			end
			if RenWuList[num][5] == false then
				if char.getWorkInt(_talkerindex,"组队") > 0 then
					char.TalkToCli(_talkerindex, _meindex, "不可组队传送！", 6);
					return;
				end
			end
			char.AllWarpToSpecificPoint(_talkerindex, RenWuList[num][2], RenWuList[num][3], RenWuList[num][4]);
		elseif _select == 16 then
			ShowDlg(_meindex, _talkerindex, _seqno - 1, RenWuList,400);
		elseif _select == 32 then
			ShowDlg(_meindex, _talkerindex, _seqno + 1, RenWuList,400);
		end
	elseif _seqno >= 800 and _seqno < 900 then
		local num = other.atoi(_data) + ((_seqno - 800) * 7);
		local num2 = char.getWorkInt(_talkerindex,"临时整数1");
		if num2 > table.getn(PetList) then--防止溢出
			return ;
		end
		if num > table.getn(PetList[num2][2]) then--防止溢出
			return ;
		end
		if PetList[num2][2][num][5] == false then
			if char.getWorkInt(_talkerindex,"组队") > 0 then
				char.TalkToCli(_talkerindex, _meindex, "不可组队传送！", 6);
				return;
			end
		end
		char.AllWarpToSpecificPoint(_talkerindex, PetList[num2][2][num][2], PetList[num2][2][num][3], PetList[num2][2][num][4]);
	end
end

function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function mydata()
	--说明,地图,X,Y,可否组队
	LianJiList = {{"2 0级乌　龟", 10001, 40, 3,true}
				 ,{"4 0级海主人", 31401, 92, 27,true}
				 ,{"6 0级布伊区", 31705, 26, 29,true}
				 ,{"8 0级大象区", 300, 81, 270,true}
				 ,{"100级蓝雷区", 32018, 12, 34,true}
				 ,{"120级雷　龙", 7402, 155,75,true}
				 ,{"140级地雷区", 60050, 15, 18,true}
				 ,{"140级金暴区", 60054, 42, 50,true}
				 ,{"160级泪之海", 817, 24,116,true}
				 ,{"阿布洞挂技能", 10007, 29, 38,true}
				 ,{"120级英雄岛",	 500, 212, 342,true}
				 };
	ZhuanYList = {{"萨姆吉尔庄园", 1040,46,29,true}
				 ,{"玛丽娜丝庄园", 2030,59,41,true}
				 ,{"加  加  庄园", 3030,59,36,true}
				 ,{"卡鲁它那庄园", 4030,29,18,true}
				 };
	ChengList = {{"荤捞呈胶", 1000, 92, 99, true}
				,{"荤捞呈胶", 2000, 65, 58, true}
				,{"荤捞呈胶", 1100, 48, 91, true}
				,{"荤捞呈胶", 1200, 61, 46, true}
				,{"荤捞呈胶", 1300, 27, 23, true}
				,{"荤捞呈胶", 1400, 74,104, true}
				,{"鲤风", 3000, 94, 97, true}
				,{"鲤风", 4000, 66, 55, true}
				,{"鲤风", 3100, 81, 70, true}
				,{"鲤风", 3200, 47, 60, true}
				,{"鲤风", 3300, 69, 51, true}
				,{"鲤风", 3400, 63, 48, true}
				,{"技福畴胶", 6000, 44, 73, true}
				,{"啊快赴", 5000, 45, 53, true}
				,{"啊快赴", 5106, 20, 36, true,"达那村"}
				,{"俊掸", 7000, 35,128, true}
				,{"俊掸", 7112, 18,  6, true,"塔耳塔地城"}
				,{"俊掸", 7200, 37, 33, true}
				,{"俊掸", 7300, 87,101, true}
				,{"俊掸", 7400, 99, 90, true}
				,{"捞悼阂啊", 1000, 18, 22, true}
				,{"器风",  500,274,369, true}
				,{"器风", 5543, 22, 22, true,"米达兰机场"}
				};
	RenWuList = {{"英雄岛前传", 5500, 10, 18, false}
				,{"英雄岛前传", 5510, 50, 17, false}
				,{"黑蛙任务", 30602, 47, 24, false}
				,{"黑蛙任务", 30604, 20, 55, false}
				,{"天空任务", 30619, 19,23, false}
				,{"天空任务", 30689, 44, 29, false}
				,{"PC老大", 5582, 58, 21, false}
				,{"席格房间", 60044, 6,7, false}
				,{"神秘老人", 60033, 14, 12, false}
				,{"大海怪前", 60208, 18, 13, false}
				,{"师傅的房间", 3008, 24, 14, false}
				,{"猜谜12楼", 850, 23, 15, false}
				,{"蒙达村", 7350, 33, 28, false}
				,{"蒙达山", 7352, 70, 61, false}
				,{"小猪森林", 703, 72, 189, false}
				,{"火山道场", 7301, 47, 28, false}
				,{"火山道场21层", 60342, 9, 11, false}
				,{"梦幻洞", 42, 33, 44, false}
				,{"地精灵洞", 60101, 7, 33, false}
				,{"海底洞窟前", 817, 188, 513, false}
				,{"天空任务", 30691, 85,60, false}
				,{"转生抓守护兽", 32012, 30,11, false}
				};
	PetList = {{"【 人龙抓捕区 】",
				{
				 {"利则诺顿(绿人龙)",21013,15,10,true},
				 {"扬奇洛斯(蓝人龙)",300,526,101,true},
				 {"邦 奇 诺(红人龙)",300,441,104,true},
				}
			   },	
			   {"【 老虎抓捕区 】",
				{
				 {" 贝 鲁 卡 (绿虎)",200,471,636,true},
				 {" 贝鲁伊卡 (黄虎)",200,350,700,true},
				 {" 格鲁西斯 (红虎)",300,229,354,true},
				}			
			   },
			   {"【 雷龙抓捕区 】",
				{
				 {"  布洛多斯 (咖啡雷)",5507,46,24,true},
				 {" 布林帖斯 (蓝雷)",32018,7,40,true},
				 {" 邦恩多斯 (金雷)",500,500,517,true},
				 {"布拉奇多斯(红雷)",400,39,100,true},
				}
			   },
			   {"【 鲨鱼抓捕区 】",
				{
				 {" 加  格 (全水鲨)",10203,11,32,true},
				 {"加 克 拉(水5火5)",10201,20,30,true},
				 {"邦 恩 吉(水8地2)",200,132,716,true},
				 {"克邦凯斯(水4地6)",200,231,563,true}
				}
			   },
			   {"【 小鸡抓捕区 】",
				{
				 {" 奇  宝 (火8风2)",100,608,206,true},
				 {" 火  鸡 (火3风7)",100,599,216,true},
				 {"霍 尔 克(地3风7)",200,624,694,true},
				 {"克克洛斯(全风鸡)",200,624,697,true}
				}		
			   },
			   {"【 猩猩抓捕区 】",
				{
				 {"格 尔 顿(地猩猩)",300,170,350,true},
				 {"奇 拉 顿(地8水2)",31204,30,53,true},
				 {"格 尔 格(火9风1)",31204,55,45,true},
				 {"齐尔格尔顿(水火)",300,238,75,true}
				}	
			   },
			   {"【 暴龙抓捕区 】",
				{
				 {"奥卡洛斯(绿暴)",31806,39,38,true},
				 {"斑尼迪克(金暴)",60054,46,20,true},
				}	
			   },
			   {"【 舌头抓捕区 】",
				{
				 {"贝 洛 恩(水舌头)",10101,12,40,true},
				 {"贝洛洛克(地7水3)",10101,19,14,true},
				 {"贝洛宝利(水3火7)",10402,44,15,true},
				 {"贝洛宝克尔(地水)",20507,42,22,true}
				}
			   },
			   {"【 大象抓捕区 】",
				{
				 {"玛 恩 摩(地9水1)",31906,25,26,true},
				 {"恩 摩 摩(水2火8)",31903,23,11,true},
				 {"玛摩那斯(地7水3)",300,136,42,true},
				 {"玛恩摩洛斯(水火)",300,104,144,true}
				}
			   },
			   {"【 海底抓捕区 】",
				{
				 {"史 菲 洛(火8风2)",817,159,83,true},	
				}
			   }
			  };
	npcdata = {"贴心传送",100000,777,37,33,6,"TieXin"};								
end

------------------------------- GM命令函数 -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.setChar(npcindex, "名字",npcdata[1]);
		char.setInt(npcindex, "图像号", npcdata[2]);
		char.setInt(npcindex, "原图像号", npcdata[2]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "方向", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 6);
	end
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 1, "["..npcdata[7].."]");
end

