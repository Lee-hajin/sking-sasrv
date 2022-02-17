function RedMoneyFunction(_charaindex, _data)
	char.TalkToCli(_charaindex, -1, "RedMoneyFunction=".._data, "��ɫ")
	local cdkey = char.getChar(_charaindex,"�˺�")
	local name = char.getChar(_charaindex,"����")
	local Tmtype = other.atoi(other.getString(_data, "|", 2))
	local num = other.atoi(other.getString(_data, "|", 3))
	local total = other.atoi(other.getString(_data, "|", 4))
	local sum = total
	local count = 0
	for i = 1,num do
		local count = 0
		if Tmtype == 1 then
			count = getIntPart(total/num)
		else
			if i == num then
				count = sum
			else
				local min = 5;--���ٸ�5��
				local safe_total = (sum - (num - i)*min)/(num-i)
				count = getIntPart(other.Random(min * 100,safe_total*100)/100)
				sum = sum - count
			end
		end
		--char.TalkToCli(_charaindex, -1, "���"..i.."�Ľ��Ϊ"..count.."�����"..sum, "��ɫ")
		--print("���"..i.."�Ľ��Ϊ"..count.."�����"..sum.."\n");
		repeat
			XandY = map.RandXAndY(MapId)
		until
			XandY > -1

		fx = map.getX(XandY)
		fy = map.getY(XandY)
		--�ж������Ƿ���ȷ
		if fx > -1 and fy > -1 then
			CreateBox(MapId, fx, fy, cdkey,name,count)
		end
	end

	--char.talkToServer(-1, "[�콵���]��ҡ�"..char.getChar(_charaindex,"����").."�����Ժ�ˬ����������˹ȫ��������"..num.."�����������"..total.."���ң���ҿ��ȥ�Ұɣ�", "��ɫ")

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

function CreateBox(fl, x, y, cdkey,name,count)
	local npcindex = npc.CreateNpc("���", moneyimg, fl, x, y, 0)
	if char.check(npcindex) == 1 then
		char.setFunctionPointer(npcindex, "ѭ���¼�", "LoopBox", "")
		char.setFunctionPointer(npcindex, "�Ի��¼�", "TalkedBox", "")
		char.setFunctionPointer(npcindex, "�����¼�", "WindowTalkedBox", "")
		char.setInt(npcindex, "ѭ���¼�ʱ��", 2000)
		char.setWorkChar(npcindex, "NPC��ʱ1", cdkey)
		char.setWorkChar(npcindex, "NPC��ʱ2", name)
		char.setInt(npcindex, "����", count)
		char.setWorkInt(npcindex,"NOTICE",120137)
	--	print("\n count= "..char.getWorkInt(npcindex, "NPC��ʱ1"))
	end
end

function CreateAllBox()
	for i = 1,table.getn(boxlist) do
		repeat
			XandY = map.RandXAndY(MapId)
		until
			XandY > -1

		fx = map.getX(XandY)
		fy = map.getY(XandY)
		--�ж������Ƿ���ȷ
		if fx > -1 and fy > -1 then
			CreateBox(MapId, fx, fy, boxlist[i])
		end
	end
end

function TalkedBox(meindex, talkerindex, szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then
		token = "\n\n�Ƿ����ǰ�ĺ����"
		lssproto.windows(talkerindex, "�Ի���", "ȷ��|ȡ��", 1, char.getWorkInt( meindex, "����"), token)
	end
end

function WindowTalkedBox( meindex, talkerindex, seqno, select, data)
	if npc.isFaceToFace(meindex, talkerindex) == 1 then
		if seqno == 1 then
			if select == 1 then
				local cdkey = char.getWorkChar(meindex, "NPC��ʱ1")
				local name = char.getWorkChar(meindex, "NPC��ʱ2")
				local count = char.getInt(meindex, "����")
				local ownerindex = getCharaindexFromCdkey(cdkey)
				if char.check(ownerindex) == 1 then
					char.TalkToCli(ownerindex, -1, "ϵͳ�����["..char.getChar(talkerindex, "����").."]��ȡ����"..count .."������", "��ɫ")
				end
				sasql.setVipPoint(talkerindex, sasql.getVipPoint(talkerindex) + count);
				char.TalkToCli(talkerindex, -1, "��ϲ���ɹ���ȡ���[".. name .. "]���͵ĺ��"..count.."�㣡", "��ɫ")
				npc.DelNpc(meindex)
			end
		end
	end
end

function LoopBox(meindex)
	if strat == 1 then
		local timer = char.getWorkInt(meindex,"NPC��ʱ2")
		timer = timer+1
		char.setWorkInt(meindex,"NPC��ʱ2",timer)

		if math.mod(timer,(warptime*60/2)) == 0 then --��ʱ�ı�λ��
			repeat
				XandY = map.RandXAndY(MapId)
			until
				XandY > -1
			fx = map.getX(XandY)
			fy = map.getY(XandY)
			--�ж������Ƿ���ȷ
			if fx > -1 and fy > -1 then
				char.WarpToSpecificPoint(meindex,MapId,fx,fy)
			end
		end
	else
		npc.DelNpc(meindex)
	end
end


function getCharaindexFromCdkey(cdkey)
	local i = 0
	local maxplayer = char.getPlayerMaxNum()
	for i = 0, maxplayer - 1 do
		if char.check(i) == 1 then
			if char.getChar(i, "�˺�") ==  cdkey then
				return i
			end
		end
	end
	return -1
end

function redmoney(_charaindex, _data)
	local Tmtype = other.atoi(other.getString(_data, " ", 1))
	local num  = other.atoi(other.getString(_data, " ", 2))
	local total  = other.atoi(other.getString(_data, " ", 3))
	RedMoneyFunction(_charaindex,"0|"..Tmtype.."|"..num.."|"..total);
end

function mydata()
	strat = 1;
	--��ͼ��
	MapId = 100;
	--ÿ�����Ӹı�һ��λ�� ����
	warptime = 5;
	--��ʱɾ�� ����
	deltime = 60;
	--�������
	moneyimg = 56469;
end

function main()
	mydata();
	magic.addLUAListFunction("redmoney", "redmoney", "", 1, "[gm redmoney ָ��]")
end
