function DelRide(_charaindex, _data)--�Ƴ�����Ȩ��
	local rideid = other.atoi(other.getString(_data, " ", 1));
	local cdkey = other.getString(_data, " ", 2);
	local toindex = -1;
	if cdkey == "" then
		toindex = _charaindex;
	else
		local maxplayer = char.getPlayerMaxNum();
		for i = 0, maxplayer - 1 do
			if char.check(i) == 1 then
				local tmcdkey = char.getChar(i, "�ʺ�");
				if cdkey == tmcdkey then
					toindex = i;
					break ;
				end
			end
		end
		if toindex <= -1 then
			char.TalkToCli(_charaindex, -1, "���ʺ���Ҳ������ϣ�", "��ɫ");
			return ;
		end
	end
	char.DelRide(toindex,rideid);
	if toindex ~= _charaindex then
		char.TalkToCli(_charaindex, -1, "�Ƴ����["..char.getChar(toindex,"����").."] "..rideid.." ������Ȩ�޳ɹ���", "��ɫ");
		char.TalkToCli(toindex, -1, "GM�Ƴ����� "..rideid.." ������Ȩ�ޣ�", "��ɫ");
	else
		char.TalkToCli(_charaindex, -1, "�Ƴ���"..rideid.."������Ȩ�޳ɹ���", "��ɫ");
	end
	return 0;
end


function GiveRide(_charaindex, _data)--��������Ȩ��
	local rideid = other.atoi(other.getString(_data, " ", 1));
	local cdkey = other.getString(_data, " ", 2);
	local toindex = -1;
	if cdkey == "" then
		toindex = _charaindex;
	else
		local maxplayer = char.getPlayerMaxNum();
		for i = 0, maxplayer - 1 do
			if char.check(i) == 1 then
				local tmcdkey = char.getChar(i, "�ʺ�");
				if cdkey == tmcdkey then
					toindex = i;
					break ;
				end
			end
		end
		if toindex <= -1 then
			char.TalkToCli(_charaindex, -1, "���ʺ���Ҳ������ϣ�", "��ɫ");
			return ;
		end
	end
	char.GiveRide(_charaindex,rideid);
	if toindex ~= _charaindex then
		char.TalkToCli(_charaindex, -1, "Ϊ���["..char.getChar(toindex,"����").."]������ "..rideid.." ������Ȩ�޳ɹ���", "��ɫ");
		char.TalkToCli(toindex, -1, "GMΪ�������� "..rideid.." ������Ȩ�ޣ�", "��ɫ");
	else
		char.TalkToCli(_charaindex, -1, "������"..rideid.."������Ȩ�޳ɹ���", "��ɫ");
	end
	return 0;
end


function main()
	magic.addLUAListFunction("�Ƴ�����", "DelRide", "", 3, " ");
	magic.addLUAListFunction("��������", "GiveRide", "", 3, " ");
end
