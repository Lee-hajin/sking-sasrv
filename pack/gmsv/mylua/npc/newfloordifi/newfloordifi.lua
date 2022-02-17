function checkEmptPetNum(charaindex)
	EmptyPetNum = 0;
	for i = 0, 4 do
		if char.check(char.getCharPet(charaindex, i)) == 0 then
			EmptyPetNum = EmptyPetNum + 1;
		end
	end
	return EmptyPetNum;
end

function checkEmptItemNum(charaindex)
	EmptyItemNum = 0;
	for i = 9, 23 do
		if char.getItemIndex(charaindex, i) == -1 then
			EmptyItemNum = EmptyItemNum + 1;
		end
	end
	return EmptyItemNum;
end

function BattleOver(meindex, battleindex, iswin)
	for i=0, 4 do
		charaindex = battle.getCharOne(battleindex, i, 0);
		if char.check(charaindex) == 1 then
			local npcid = char.getWorkInt(meindex, "��ʱ����1");
			char.setInt(charaindex, "¥��ʱ��", 20 * 60);
			if char.getFlg(charaindex, "����") == 1 then
				char.DischargeParty(charaindex, 1);
				char.WarpToSpecificPoint(charaindex, npcmapdata[1], npcmapdata[2], npcmapdata[3]);
			end
			if iswin == 1 then
				char.setWorkInt(charaindex, "��ʱ����1", npcid);
				char.TalkToCli(charaindex, meindex, "���Ǻ���������Ȼ���Ҹ�����ˣ�", "��ɫ");
			end
		end
	end
end


--num = math.floor(math.random(1, 10000));
--		NLG.ShowWindowTalked(_PlayerIndex, 1, 3, 1, "               ħ����֤��:["..num.."] \n\n    ��ӭ����սħ�����ɹ���սħ���㽫��õ���VIP����͵���Ŷ��ǰ���Ǳ���Ҫ����ƱŶ���ſ��Խ���Ŷ\n\n���������������֤�룺\n\n", _MeIndex); 
--end

function question(meindex, talkerindex, seqno, token)
	num = char.setWorkInt(talkerindex, "��ʱ��", math.random(8999)+1000);
 -- num = math.floor(math.random(1, 3));
	local floorid = char.getWorkInt(meindex, "��ʱ����1");
	local token = "";
	if floorid > 0 then
		token = "    ��ӭ����սħ����"
				.."\n�ɹ���սħ���㽫��ó���͵���Ŷ��"
				.."\nǰ���Ǳ���Ҫ���㹻������Ŷ��"
				.."\n��ս��¥��Ҫ[style c=4]"..npctl[floorid].."[/style]����"
				.."\n���������������֤�룺"
				.."\n\n������ħ����֤��:��[style c=5]"..num.."[/style]��\n\n";
	else
		token = "    ��ӭ����սħ����"
				.."\n�ɹ���սħ���㽫��ó���͵���Ŷ��"
				.."\nǰ���Ǳ���Ҫ���㹻������Ŷ��"
				.."\n���������������֤�룺"
				.."\n\n������ħ����֤��:��[style c=5]"..num.."[/style]��\n\n";
	end
	lssproto.windows(talkerindex, "�����", 12, seqno, char.getWorkInt( meindex, "����"), token, char.getWorkInt(talkerindex, "��ʱ��"));
end


--NPC�Ի��¼�(NPC����)
function Talked(meindex, talkerindex , szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then 
		if char.getWorkInt(meindex, "��ʱ����1") == 0 then
			token = "3                 ��" .. char.getChar(meindex, "����") .. "��\n"
						 .. "\n��Ҫ����������ս��??"
						 .. "\n                   ��Ҫ������ս"
						 .. "\n                   ��Ҫ���й�ս"
						 .. "\n                   �鿴����ʱ��"
						 .. "\n                   ���̽���ʱ��";
			lssproto.windows(talkerindex, "ѡ���", "ȡ��", 1, char.getWorkInt( meindex, "����"), token);
		else
			if char.getWorkInt(talkerindex, "��ʱ����1") ~= char.getWorkInt(meindex, "��ʱ����1") then
				token = "��Ҫ���ܸ�¥���������ս��??" ;
				question(meindex, talkerindex, 0, token);
			else
				token = "4                 ��" .. char.getChar(meindex, "����") .. "��\n\n"
							 .. "��̫�����ˣ����Ѿ��������ˣ���������\n"
							 .. "\n             ����ǰ������һ�ؿ�\n"
							 .. "\n             ��ȡ�������˳���ս";
				lssproto.windows(talkerindex, "ѡ���", "ȡ��", 0, char.getWorkInt( meindex, "����"), token);
			end
		end
	end
end

--NPC�����¼�(NPC����)
function WindowTalked ( meindex, talkerindex, _seqno, _select, _data)
	if _select == 8 then
		return;
	end
	if npc.isFaceToFace(meindex, talkerindex) == 1 then 
		if _seqno == 0 then
			if char.getWorkInt(talkerindex, "��ʱ����1") ~= char.getWorkInt(meindex, "��ʱ����1") then
				
		
					id = char.getWorkInt(meindex, "��ʱ����1");
					if id < 1 or id > 20 then
						char.TalkToCli(talkerindex, -1, "ϵͳ��������,���ڹ���Ա��ϵ", "��ɫ");
						return;
					end
		
					if char.getWorkInt(talkerindex, "���") == 2 then
						return;
					end
	
					battleindex = battle.CreateVsEnemy(talkerindex, meindex, enemytable[id]);
					char.setWorkInt(meindex,"��ֹ���",1);
					char.setWorkInt(talkerindex,"��ֹ���",1);
					char.setWorkInt(meindex,"��ֹ����",1);
					char.setWorkInt(talkerindex,"��ֹ����",1);
					char.TalkToCli(talkerindex, meindex, "��B����ս�磬�㲻����ˣ������㣡", "��ɫ");
			
					char.setWorkInt(talkerindex, "��ʱ��", math.random(8999)+1000);
			
			else
				local num = other.atoi(_data)
				if num == 1 then
					token = "��Ҫ������һ����?��ش�������֤��??";
					question(meindex, talkerindex, 10, token);
				elseif num == 3 then
					token = "��Ҫ��ȡ������?��ش�������֤��??";
					question(meindex, talkerindex, 11, token);
				end
			end
		elseif _seqno == 1 then
			if _select == 0 then
				local num = other.atoi(_data);
				if num == 1 then
					token = "��Ҫ����¥����ս��?��ش�������֤��??";
					question(meindex, talkerindex, 12, token);
				elseif num == 2 then
					token = "                 ��" .. char.getChar(meindex, "����") .. "��\n\n"
							 .. "����������Ҫ��ս��¥�㣬�����ս1¥������1�������ս2¥������2";
					lssproto.windows(talkerindex, "�����", "ȷ��|ȡ��", 2, char.getWorkInt( meindex, "����"), token);
				elseif num == 3 then
					if char.getInt(talkerindex, "¥��ʱ��") > 0 then
						char.TalkToCli(talkerindex, meindex, "����" .. char.getInt(talkerindex, "¥��ʱ��") .. "���������ս��", "��ɫ");
					else
						char.TalkToCli(talkerindex, meindex, "�������Ѿ����Խ�����ս�ˣ�", "��ɫ");
					end
				elseif num == 4 then
					if char.getInt(talkerindex, "¥��ʱ��") > 0 then
						token = "                 ��" .. char.getChar(meindex, "����") .. "��\n\n"
								 .. "����Ҫ���̽���ȴ�ʱ����ÿ����1���������֧��1����Ӵ���㵱ǰ�ȴ�ʱ����" .. char.getInt(talkerindex, "¥��ʱ��") .. "�룬��������Ҫ���������룿��";
						lssproto.windows(talkerindex, "�����", "ȷ��|ȡ��", 3, char.getWorkInt( meindex, "����"), token);
					else
						char.TalkToCli(talkerindex, meindex, "��ǰ����������ʱ�伴�ɽ��룡", "��ɫ");
					end
				end
			end
		elseif _seqno == 2 then
			if _select == 1 then
				local num = other.atoi(_data);
				if num < 1 then
					num = 1;
				elseif num > 20 then
					num = 20;
				end
				if char.check(npcindex[num]) == 1 then
					if char.getWorkInt(npcindex[num], "ս��") ~= 0 then
						battle.WatchEntry(talkerindex, npcindex[num]);
						return;
					end
				end
				char.TalkToCli(talkerindex, meindex, num .. "¥��û��ս����", "��ɫ");
			end
		elseif _seqno == 3 then
			if _select == 1 then
				local num = other.atoi(_data);
				if num > 0 then
					if num > char.getInt(talkerindex, "¥��ʱ��") then
						num = math.max(1, char.getInt(talkerindex, "¥��ʱ��"));
					end
					if char.getInt(talkerindex, "����") < num * 100 then
						char.TalkToCli(talkerindex, meindex, "�㵱ǰ����������������" .. num .. "��", "��ɫ");
					else
						char.setInt(talkerindex, "¥��ʱ��", char.getInt(talkerindex, "¥��ʱ��") - num);
						char.setInt(talkerindex, "����", char.getInt(talkerindex, "����") - num * 100);
						char.TalkToCli(talkerindex, meindex, "�㵱ǰ������" .. num .. "�룬�۳�" .. num .. "����", "��ɫ");
					end
				else
					char.TalkToCli(talkerindex, meindex, "���������������ʱ�䣡", "��ɫ");
				end
			end
		elseif _seqno == 10 then 
			if other.atoi(_data) == char.getWorkInt(talkerindex, "��ʱ��") then
				if char.getWorkInt(talkerindex, "���") ~= 0 then
					char.TalkToCli(talkerindex, meindex, "���ɢ�Ŷӣ��Ҳ���Ϊ�㴫����ȥ��", "��ɫ");
					return
				end
		
				local npcid = char.getWorkInt(meindex, "��ʱ����1");
				
				if npcid == maxfloord then
					char.TalkToCli(talkerindex, meindex, "�����Ѿ������һ���ˣ���ֱ���콱Ʒ�ɣ�", "��ɫ");
					return
				end
				char.WarpToSpecificPoint(talkerindex, 40001+npcid, 5, 6);
				
			end
		elseif _seqno == 11 then
			if other.atoi(_data) == char.getWorkInt(talkerindex, "��ʱ��") then
				if checkEmptItemNum(talkerindex) == 0 then
					char.TalkToCli(talkerindex, meindex, "��Ʒ������������������㹻�Ŀ�λ��", "��ɫ");
					return;
				end
				local npcid = char.getWorkInt(meindex, "��ʱ����1");
				local rnd = math.random(100);
				if npcid == 1 then
					jl = 4;
				elseif npcid == 2 then
					jl = 8;
				elseif npcid == 3 then
					jl = 12;
				elseif npcid == 4 then
					jl = 20;
				end
				if rnd <= jl then
					itemidmin = awardlist[npcid][1][1];
					itemidmax = awardlist[npcid][1][2];
					itemid = math.random(itemidmin, itemidmax);
					npc.AddItem(talkerindex, itemid)
					char.talkToAllServer("[������ս��Ѷ]��ϲ���� " .. char.getChar(talkerindex, "����")  .. " ��������ս��" .. npcid .. "���л��"  .. item.getNameFromNumber(itemid),4);
				elseif rnd <= 70 + jl then
					char.setInt(talkerindex, "ˢ¥����", char.getInt(talkerindex, "ˢ¥����") + npcid*2);
					char.talkToAllServer("[¥����ս��Ѷ]��ϲ���� " .. char.getChar(talkerindex, "����")  .. " ��¥����ս��" .. npcid .. "�ػ��".. npcid*2 .."ˢ¥����",4);
				else
					char.setInt(talkerindex, "����", char.getInt(talkerindex, "����") + npcid * 10);
					char.talkToAllServer("[������ս��Ѷ]��ϲ���� " .. char.getChar(talkerindex, "����")  .. " ��������ս��" .. npcid .. "�ػ��" .. npcid * 10 .. "�����!",4);
				end
				char.WarpToSpecificPoint(talkerindex, npcmapdata[1], npcmapdata[2],npcmapdata[3]);
			end
		elseif _seqno == 12 then
			if other.atoi(_data) == char.getWorkInt(talkerindex, "��ʱ��") then
				if char.getWorkInt(talkerindex, "���") ~= 0 then
					char.TalkToCli(talkerindex, -1, "���ɢ�ŶӺ���룡", "��ɫ");
					return;
				end

				if char.getInt(talkerindex, "¥��ʱ��") > 0 then
					char.TalkToCli(talkerindex, meindex, "����" .. char.getInt(talkerindex, "¥��ʱ��") .. "���������ս��", "��ɫ");
					return;
				end
				
				--if npc.Free(meindex, talkerindex, "ITEM=25500") == 1 then
					--npc.DelItem(talkerindex, "25500*1");
				--else
					--char.TalkToCli(talkerindex, meindex, "�����ϲ�û��������ս��Ʊ���޷��������!", "��ɫ");
					--return;
				--end
				
				char.setInt(talkerindex, "¥��ʱ��", 20 * 60);
				
				char.talkToAllServer( "[������ս��Ѷ]���� " .. char.getChar(talkerindex, "����") .. " �μ�������ս����Ҳ�����ɣ�", "��ɫ");
				char.WarpToSpecificPoint(talkerindex, 40001, 5, 7);
				char.setWorkInt(talkerindex, "��ʱ����1", 0);
			end
		end
	end
end

function getIntPart(x)
    if x <= 0 then
       return math.ceil(x);
    end

    if math.ceil(x) == x then
       x = math.ceil(x);
    else
       x = math.ceil(x) - 1;
    end
    return x;
end

function mydata()
	awardtype = {"����", "��Ʒ"};

	awardlist = {{{25501, 25596}, {24333, 24333}, {24157, 24158}, {24151, 24156}}
				,{{25501, 25596}, {24333, 24333}, {24157, 24158}, {24151, 24153}}
				,{{25501, 25596}, {24333, 24333}, {24157, 24158}, {24151, 24156}}
				,{{25501, 25596}, {24333, 24333}, {24157, 24158}, {24151, 24153}}
				};
	
	changeaward = {{2, 25802, {{25501, 25502, 25503, 25504}, 3000000, 15000}} --�׻�
				  ,{2, 25801, {{25505, 25506, 25507, 25508}, 3500000, 15000}} --ˮ��
				  ,{2, 25803, {{25509, 25510, 25511, 25512}, 2000000, 10000}} --������
				  ,{2, 25804, {{25513, 25514, 25515, 25516}, 3000000, 15000}} --����
				  ,{2, 25805, {{25517, 25518, 25519, 25520}, 4000000, 20000}} --����
				  ,{2, 25806, {{25521, 25522, 25523, 25524}, 3000000, 10000}} --��ħ
				  ,{2, 25807, {{25525, 25526, 25527, 25528}, 2000000, 10000}} --��ţ
				  ,{2, 25808, {{25529, 25530, 25531, 25532}, 2000000, 10000}} --��¹
				  ,{2, 25809, {{25533, 25534, 25535, 25536}, 2000000, 10000}} --�۷�
				  ,{2, 25810, {{25537, 25538, 25539, 25540}, 4000000, 20000}} --ţ��
				  ,{2, 25811, {{25541, 25542, 25543, 25544}, 3000000, 15000}} --è��
				  ,{2, 25812, {{25545, 25546, 25547, 25548}, 4000000, 20000}} --����
				  ,{2, 25813, {{25549, 25550, 25551, 25552}, 2000000, 10000}} --�׳�
				  ,{2, 25814, {{25553, 25554, 25555, 25556}, 3000000, 15000}} --��ȸ
				  ,{2, 25815, {{25557, 25558, 25559, 25560}, 3000000, 15000}} --����
				  ,{2, 25816, {{25561, 25562, 25563, 25564}, 2000000, 10000}} --����
				  ,{2, 25817, {{25565, 25566, 25567, 25568}, 2000000, 10000}} --˫��
				  ,{2, 25818, {{25569, 25570, 25571, 25572}, 2000000, 10000}} --ӥ��
				  --,{1, 2565, {{25573, 25574, 25575, 25576}, 3000000, 15000}} --����
				  ,{2, 25819, {{25573, 25574, 25575, 25576}, 3000000, 15000}} --����
				  ,{2, 25832, {{25577, 25578, 25579, 25580}, 4000000, 15000}} --����
				  ,{2, 25833, {{25581, 25582, 25583, 25584}, 4000000, 20000}} --����
				  ,{2, 25834, {{25585, 25586, 25587, 25588}, 4000000, 20000}} --����
				  ,{2, 25837, {{25589, 25590, 25591, 25592}, 1000000, 2000}}  --����
				  ,{2, 25838, {{25593, 25594, 25595, 25596}, 3000000, 15000}} --����
				  ,{2, 26017, {{25597, 25598, 25599, 25600}, 6000000, 15000}} --�׻���֤
				  ,{2, 26019, {{25601, 25602, 25603, 25604}, 6000000, 15000}} --����֤
				  ,{2, 26020, {{25605, 25606, 25607, 25608}, 6000000, 15000}} --�ڱ���֤
				  ,{2, 26021, {{25609, 25610, 25611, 25612}, 6000000, 15000}} --������֤
				  };
	maxpage = getIntPart(table.getn(changeaward) / 6) + 1;
	enemytable = {{1, 2, 3, 4, -1, -1, -1, -1, -1, -1}
				 ,{2956, 2963, 2964, 2965, 2966, -1, -1, -1, -1, -1}
				 ,{2957, 2967, 2968, 2969, 2970, -1, -1, -1, -1, -1}
				 ,{2958, 2971, 2972, 2973, 2974, -1, -1, -1, -1, -1}
				 };
	npcindex = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	maxfloord = 7;--���ŵ��ܲ���
	npcdata = {{"�������޵�",101996,777,47,45,6,0}
			  ,{"���޵�1F",100000,40001,5,5,4,1}
			  ,{"���޵�2F",100000,40002,5,5,4,2}
			  ,{"���޵�3F",100000,40003,5,5,4,3}
			  ,{"���޵�4F",100000,40004,5,5,4,4}
			  ,{"���޵�5F",100000,40005,5,5,4,5}
			  ,{"���޵�6F",100000,40006,5,5,4,6}
			  ,{"���޵�7F",100000,40007,5,5,4,7}
			  ,{"���޵�8F",100000,40008,5,5,4,8}
			  ,{"���޵�9F",100000,40009,5,5,4,9}
			  ,{"���޵�10F",100000,40010,5,5,4,10}
			  };
	npcmapdata = {2005,13,13};
	npctl = {2,2,2,5,5,8,8,10,10,10};
end

function Create(name, metamo, floor, x, y, dir, flg, lv)
	local index = npc.CreateNpc(name, metamo, floor, x, y, dir);
	char.setFunctionPointer(index, "�Ի��¼�", "Talked", "");
	char.setFunctionPointer(index, "�����¼�", "WindowTalked", "");
	char.setFunctionPointer(index, "ս���¼�", "BattleOver", "");
	char.setInt(index, "�ȼ�", lv);
	char.setWorkInt(index, "��ʱ����1", flg);
	if flg > 0 and flg <= table.getn(npcindex) then
		npcindex[flg] = index;
	end
end

function reloadnpc()
	mydata();
	for i=1,table.getn(npcindex) do
		if npcindex[i] >= 0 then
			print("�ض�NPC��Ϣ",char.getChar(npcindex[i], "����"));
			char.setChar(npcindex[i], "����",npcdata[i][1]);
			char.setInt(npcindex[i], "ͼ���", npcdata[i][2]);
			char.setInt(npcindex[i], "ԭͼ���", npcdata[i][2]);
			char.WarpToSpecificPoint(npcindex[i], npcdata[i][3], npcdata[i][4], npcdata[i][5]);
			char.setInt(npcindex[i], "����", npcdata[i][6]);
			char.ToAroundChar(npcindex[i]);
		end
	end
	
end

function main()
	mydata();
	--Create(npcdata[1][1], npcdata[1][2], npcdata[1][3], npcdata[1][4], npcdata[1][5], npcdata[1][6], npcdata[1][7], 140);
	--Create(npcdata[2][1], npcdata[2][2], npcdata[2][3], npcdata[2][4], npcdata[2][5], npcdata[2][6], npcdata[2][7], 140);
	--Create(npcdata[3][1], npcdata[3][2], npcdata[3][3], npcdata[3][4], npcdata[3][5], npcdata[3][6], npcdata[3][7], 140);
	--Create(npcdata[4][1], npcdata[4][2], npcdata[4][3], npcdata[4][4], npcdata[4][5], npcdata[4][6], npcdata[4][7], 140);
	--Create(npcdata[5][1], npcdata[5][2], npcdata[5][3], npcdata[5][4], npcdata[5][5], npcdata[5][6], npcdata[5][7], 140);
	--Create(npcdata[6][1], npcdata[6][2], npcdata[6][3], npcdata[6][4], npcdata[6][5], npcdata[6][6], npcdata[6][7], 140);
	--Create(npcdata[7][1], npcdata[7][2], npcdata[7][3], npcdata[7][4], npcdata[7][5], npcdata[7][6], npcdata[7][7], 140);
	--Create(npcdata[8][1], npcdata[8][2], npcdata[8][3], npcdata[8][4], npcdata[8][5], npcdata[8][6], npcdata[8][7], 140);
	--Create(npcdata[9][1], npcdata[9][2], npcdata[9][3], npcdata[9][4], npcdata[9][5], npcdata[9][6], npcdata[9][7], 140);
	--Create(npcdata[10][1], npcdata[10][2], npcdata[10][3], npcdata[10][4], npcdata[10][5], npcdata[10][6], npcdata[10][7], 140);
	--Create(npcdata[11][1], npcdata[11][2], npcdata[11][3], npcdata[11][4], npcdata[11][5], npcdata[11][6], npcdata[11][7], 140);
	
	magic.addLUAListFunction("�ض�ˢ¥ϵͳ", "reloadnpc", "", 1, "[�ض�ˢ¥ϵͳ]");
end
