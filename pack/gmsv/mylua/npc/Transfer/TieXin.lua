function ShowDlg(_meindex, _talkerindex, _page,_transdata,_seqno)
	local _maxpage = math.ceil(table.getn(_transdata) / 7);
	local token = "1 ��� ������ �̵��Ͻðڽ��ϱ�?��\n";
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
	lssproto.windows(_talkerindex, 2, button, _page, char.getWorkInt( _meindex, "����"), token);
end


function FreeTieXin(charaindex)
	local token = "2\n������ʲô��Ҫ�Ұ�æ��ô��\n\n"
				.."                  [style c=5]�� ����� ��[/style]\n"
				.."                  [style c=5]�� ������ ��[/style]\n"
				.."                  [style c=5]�� ׯ԰���� ��[/style]\n"
				.."                  [style c=5]�� ץ�贫�� ��[/style]\n"
				.."                  [style c=5]�� �����̵� ��[/style]";
	lssproto.windows(charaindex, 2, 8, 0, char.getWorkInt( npcindex, "����"), token);
end

--NPC�Ի��¼�(NPC����)
function Talked(_meindex, _talkerindex, _szMes, _color )
	local token = "2\n������ʲô��Ҫ�Ұ�æ��ô��\n\n"
				.."                  [style c=5]�� ����� ��[/style]\n"
				.."                  [style c=5]�� ������ ��[/style]\n"
				.."                  [style c=5]�� ׯ԰���� ��[/style]\n"
				.."                  [style c=5]�� ץ�贫�� ��[/style]\n"
				.."                  [style c=5]�� �����̵� ��[/style]";
	lssproto.windows(_talkerindex, 2, 8, 0, char.getWorkInt( _meindex, "����"), token);
end

function ShowPetType(_meindex, _talkerindex, _page)
	local _maxpage = math.ceil(table.getn(PetList) / 7);
	local topselect = (_page-10) * 7;

	local token = "1 ��ѡ����Ҫץ�ĳ�������:\n";
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
	lssproto.windows(_talkerindex, 2, button, _page, char.getWorkInt( _meindex, "����"), token);
end

--NPC�����¼�(NPC����)
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
			if num > table.getn(PetList) then--��ֹ���
				return ;
			end
			ShowDlg(_meindex, _talkerindex, 800,PetList[num][2],800);
			char.setWorkInt(_talkerindex,"��ʱ����1",num);
		elseif _select == 16 then
			ShowPetType(_meindex, _talkerindex, _seqno - 1);
		elseif _select == 32 then
			ShowPetType(_meindex, _talkerindex, _seqno + 1);
		end
	elseif _seqno >= 100 and _seqno < 200 then
		if _select == 0 then
			local num = other.atoi(_data) + ((_seqno - 100) * 7);
			if num > table.getn(LianJiList) then--��ֹ���
				return ;
			end
			if LianJiList[num][5] == false then
				if char.getWorkInt(_talkerindex,"���") > 0 then
					char.TalkToCli(_talkerindex, _meindex, "������Ӵ��ͣ�", 6);
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
			if num > table.getn(ChengList) then--��ֹ���
				return ;
			end
			if ChengList[num][5] == false then
				if char.getWorkInt(_talkerindex,"���") > 0 then
					char.TalkToCli(_talkerindex, _meindex, "������Ӵ��ͣ�", 6);
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
			if num > table.getn(ZhuanYList) then--��ֹ���
				return ;
			end
			if ZhuanYList[num][5] == false then
				if char.getWorkInt(_talkerindex,"���") > 0 then
					char.TalkToCli(_talkerindex, _meindex, "������Ӵ��ͣ�", 6);
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
			if num > table.getn(RenWuList) then--��ֹ���
				return ;
			end
			if RenWuList[num][5] == false then
				if char.getWorkInt(_talkerindex,"���") > 0 then
					char.TalkToCli(_talkerindex, _meindex, "������Ӵ��ͣ�", 6);
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
		local num2 = char.getWorkInt(_talkerindex,"��ʱ����1");
		if num2 > table.getn(PetList) then--��ֹ���
			return ;
		end
		if num > table.getn(PetList[num2][2]) then--��ֹ���
			return ;
		end
		if PetList[num2][2][num][5] == false then
			if char.getWorkInt(_talkerindex,"���") > 0 then
				char.TalkToCli(_talkerindex, _meindex, "������Ӵ��ͣ�", 6);
				return;
			end
		end
		char.AllWarpToSpecificPoint(_talkerindex, PetList[num2][2][num][2], PetList[num2][2][num][3], PetList[num2][2][num][4]);
	end
end

function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "�Ի��¼�", "Talked", "");
	char.setFunctionPointer(npcindex, "�����¼�", "WindowTalked", "");
end

function mydata()
	--˵��,��ͼ,X,Y,�ɷ����
	LianJiList = {{"2 0���ڡ���", 10001, 40, 3,true}
				 ,{"4 0��������", 31401, 92, 27,true}
				 ,{"6 0��������", 31705, 26, 29,true}
				 ,{"8 0��������", 300, 81, 270,true}
				 ,{"100��������", 32018, 12, 34,true}
				 ,{"120���ס���", 7402, 155,75,true}
				 ,{"140��������", 60050, 15, 18,true}
				 ,{"140������", 60054, 42, 50,true}
				 ,{"160����֮��", 817, 24,116,true}
				 ,{"�������Ҽ���", 10007, 29, 38,true}
				 ,{"120��Ӣ�۵�",	 500, 212, 342,true}
				 };
	ZhuanYList = {{"��ķ����ׯ԰", 1040,46,29,true}
				 ,{"������˿ׯ԰", 2030,59,41,true}
				 ,{"��  ��  ׯ԰", 3030,59,36,true}
				 ,{"��³����ׯ԰", 4030,29,18,true}
				 };
	ChengList = {{"���̳ʽ�", 1000, 92, 99, true}
				,{"���̳ʽ�", 2000, 65, 58, true}
				,{"���̳ʽ�", 1100, 48, 91, true}
				,{"���̳ʽ�", 1200, 61, 46, true}
				,{"���̳ʽ�", 1300, 27, 23, true}
				,{"���̳ʽ�", 1400, 74,104, true}
				,{"���", 3000, 94, 97, true}
				,{"���", 4000, 66, 55, true}
				,{"���", 3100, 81, 70, true}
				,{"���", 3200, 47, 60, true}
				,{"���", 3300, 69, 51, true}
				,{"���", 3400, 63, 48, true}
				,{"�����뽺", 6000, 44, 73, true}
				,{"���츰", 5000, 45, 53, true}
				,{"���츰", 5106, 20, 36, true,"���Ǵ�"}
				,{"����", 7000, 35,128, true}
				,{"����", 7112, 18,  6, true,"�������س�"}
				,{"����", 7200, 37, 33, true}
				,{"����", 7300, 87,101, true}
				,{"����", 7400, 99, 90, true}
				,{"�̵��Ұ�", 1000, 18, 22, true}
				,{"����",  500,274,369, true}
				,{"����", 5543, 22, 22, true,"�״�������"}
				};
	RenWuList = {{"Ӣ�۵�ǰ��", 5500, 10, 18, false}
				,{"Ӣ�۵�ǰ��", 5510, 50, 17, false}
				,{"��������", 30602, 47, 24, false}
				,{"��������", 30604, 20, 55, false}
				,{"�������", 30619, 19,23, false}
				,{"�������", 30689, 44, 29, false}
				,{"PC�ϴ�", 5582, 58, 21, false}
				,{"ϯ�񷿼�", 60044, 6,7, false}
				,{"��������", 60033, 14, 12, false}
				,{"�󺣹�ǰ", 60208, 18, 13, false}
				,{"ʦ���ķ���", 3008, 24, 14, false}
				,{"����12¥", 850, 23, 15, false}
				,{"�ɴ��", 7350, 33, 28, false}
				,{"�ɴ�ɽ", 7352, 70, 61, false}
				,{"С��ɭ��", 703, 72, 189, false}
				,{"��ɽ����", 7301, 47, 28, false}
				,{"��ɽ����21��", 60342, 9, 11, false}
				,{"�λö�", 42, 33, 44, false}
				,{"�ؾ��鶴", 60101, 7, 33, false}
				,{"���׶���ǰ", 817, 188, 513, false}
				,{"�������", 30691, 85,60, false}
				,{"ת��ץ�ػ���", 32012, 30,11, false}
				};
	PetList = {{"�� ����ץ���� ��",
				{
				 {"����ŵ��(������)",21013,15,10,true},
				 {"������˹(������)",300,526,101,true},
				 {"�� �� ŵ(������)",300,441,104,true},
				}
			   },	
			   {"�� �ϻ�ץ���� ��",
				{
				 {" �� ³ �� (�̻�)",200,471,636,true},
				 {" ��³���� (�ƻ�)",200,350,700,true},
				 {" ��³��˹ (�컢)",300,229,354,true},
				}			
			   },
			   {"�� ����ץ���� ��",
				{
				 {"  �����˹ (������)",5507,46,24,true},
				 {" ������˹ (����)",32018,7,40,true},
				 {" �����˹ (����)",500,500,517,true},
				 {"�������˹(����)",400,39,100,true},
				}
			   },
			   {"�� ����ץ���� ��",
				{
				 {" ��  �� (ȫˮ��)",10203,11,32,true},
				 {"�� �� ��(ˮ5��5)",10201,20,30,true},
				 {"�� �� ��(ˮ8��2)",200,132,716,true},
				 {"�˰˹(ˮ4��6)",200,231,563,true}
				}
			   },
			   {"�� С��ץ���� ��",
				{
				 {" ��  �� (��8��2)",100,608,206,true},
				 {" ��  �� (��3��7)",100,599,216,true},
				 {"�� �� ��(��3��7)",200,624,694,true},
				 {"�˿���˹(ȫ�缦)",200,624,697,true}
				}		
			   },
			   {"�� ����ץ���� ��",
				{
				 {"�� �� ��(������)",300,170,350,true},
				 {"�� �� ��(��8ˮ2)",31204,30,53,true},
				 {"�� �� ��(��9��1)",31204,55,45,true},
				 {"��������(ˮ��)",300,238,75,true}
				}	
			   },
			   {"�� ����ץ���� ��",
				{
				 {"�¿���˹(�̱�)",31806,39,38,true},
				 {"����Ͽ�(��)",60054,46,20,true},
				}	
			   },
			   {"�� ��ͷץ���� ��",
				{
				 {"�� �� ��(ˮ��ͷ)",10101,12,40,true},
				 {"�������(��7ˮ3)",10101,19,14,true},
				 {"���屦��(ˮ3��7)",10402,44,15,true},
				 {"���屦�˶�(��ˮ)",20507,42,22,true}
				}
			   },
			   {"�� ����ץ���� ��",
				{
				 {"�� �� Ħ(��9ˮ1)",31906,25,26,true},
				 {"�� Ħ Ħ(ˮ2��8)",31903,23,11,true},
				 {"��Ħ��˹(��7ˮ3)",300,136,42,true},
				 {"���Ħ��˹(ˮ��)",300,104,144,true}
				}
			   },
			   {"�� ����ץ���� ��",
				{
				 {"ʷ �� ��(��8��2)",817,159,83,true},	
				}
			   }
			  };
	npcdata = {"���Ĵ���",100000,777,37,33,6,"TieXin"};								
end

------------------------------- GM����� -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "�ض�" then
		mydata();
		char.setChar(npcindex, "����",npcdata[1]);
		char.setInt(npcindex, "ͼ���", npcdata[2]);
		char.setInt(npcindex, "ԭͼ���", npcdata[2]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "����", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "�ض�NPC["..char.getChar(npcindex, "����").."]���", 6);
	end
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 1, "["..npcdata[7].."]");
end

