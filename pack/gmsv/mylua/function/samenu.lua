function FreeSaMenu(charaindex, tonidex,index1,index2,index3)
	local battletype = char.getWorkInt(charaindex,"ս��");
	parameter = {charaindex};
	if index1 == 1 then
		if index2 == 1 then--�ٷ���վ
			lssproto.SaMenu(charaindex, "http://www.test.com");
		elseif index2 == 2 then--������ֵ
			lssproto.SaMenu(charaindex, "http://www.test.com");
		elseif index2 == 4 then--ԭ������
			if index3 == 0 then
				char.StopEncounter(charaindex);
			else
				char.Encounter(charaindex);
			end
		elseif index2 == 5 then--�������
			if index3 == 1 then
				char.setforpigcomlua(charaindex);--��������
			elseif index3 == 0 then
				char.clearforpigcomlua(charaindex);--ȡ������
			end
			--print("\n״̬:"..char.getforpigcomlua(charaindex));
		elseif index2 == 6 then--֧Ʊǩ��
			other.CallFunction("FreeZhiPiao", "mylua/npc/ZhiPiao/ZhiPiao.lua", parameter);
		elseif index2 == 7 then--����
			other.CallFunction("ItemBoxToItemBox", "mylua/function/chartalkfunction.lua", parameter);
		elseif index2 == 8 then--���Ĵ���
			other.CallFunction("FreeTieXin", "mylua/npc/Transfer/TieXin.lua", parameter);
		elseif index2 == 9 then--������Ϣ
			other.CallFunction("FreeFameShow", "mylua/function/fameshow.lua", parameter);
		elseif index2 == 10 then--����
			local fd = char.getFd(charaindex);--��ȡFD
			--lssproto.show(charaindex,char.getWorkInt(charaindex,"����"));
			net.echosend(fd);
			--char.WarpToSpecificPoint(charaindex, char.getInt(charaindex, "��ͼ��"), char.getInt(charaindex, "����X"), char.getInt(charaindex, "����Y"));
		elseif index2 == 11 then--�ٻ�
			--other.CallFunction("Accompany", "mylua/item/Accompany.lua", parameter);
			char.TalkToCli(charaindex, -1, "��δ����", "��ɫ");
		end
	elseif index1 == 2 then
	elseif index1 == 3 then
		if index2 == 1 then--���鿴
			other.CallFunction("FreeRideQuery", "mylua/function/familyridefunction.lua", parameter);
		elseif index2 == 2 then--�����̳�
			other.CallFunction("FreeShopVip", "mylua/npc/Shop/VipShop/VipShop.lua", parameter);
		elseif index2 == 3 then--ÿ��ǩ��
			other.CallFunction("ShowOneDay", "mylua/npc/OneDay/OneDay.lua", parameter);
		elseif index2 == 4 then--�ۿ�ս��
			battle.WatchEntry(charaindex,tonidex);--��ս��ť
		elseif index2 == 5 then--�ʼ�ϵͳ
			other.CallFunction("FreeEmail", "mylua/npc/Email/Email.lua", parameter);
		elseif index2 == 6 then--���ϵͳ
			other.CallFunction("FreeRedBox", "mylua/npc/RedBox/RedBox.lua", parameter);
		end
	elseif index1 == 4 then
		if index2 == 1 then
			other.CallFunction("FreeSkin", "mylua/npc/PlayerSkin/PlayerSkin.lua", parameter);
		elseif index2 == 2 then
			other.CallFunction("FreeTitle", "mylua/npc/PlayerTitle/PlayerTitle.lua", parameter);
		elseif index2 == 3 then
			other.CallFunction("FreeHalo", "mylua/npc/PlayerHalo/PlayerHalo.lua", parameter);
		end
	elseif index1 == 5 then
		if index2 == 1 then--���
			local petindex = other.atoi(index3);
			if char.check(petindex) == 1 then
				local PetA1 = char.getChar(petindex,"����");
				local PetA2 = char.getInt(petindex,"ԭͼ���");
				local PetA3 = char.getInt(petindex,"ת��");
				local PetA4 = char.getInt(petindex,"�ȼ�");
				local PetA5 = char.getInt(petindex,"�ںϳ�");
						
				local PetB1 = char.getInt(petindex,"����ȼ�");
				local PetB2 = char.getInt(petindex, "����;�");
				local PetB3 = char.getInt(petindex, "�������");
				local PetB4 = char.getInt(petindex, "�������");
				local PetB5 = char.getInt(petindex, "�������");
						
				local PetC1 = char.getWorkInt(petindex,"���HP");
				local PetC2 = char.getWorkInt(petindex,"������");
				local PetC3 = char.getWorkInt(petindex,"������");
				local PetC4 = char.getWorkInt(petindex,"������");
				
				local PetD1 = char.getInt(petindex,"��");
				local PetD2 = char.getInt(petindex,"ˮ");
				local PetD3 = char.getInt(petindex,"��");
				local PetD4 = char.getInt(petindex,"��");
				
				local PetSK = {"","","","","","",""};
				for i=0,6 do
					local TMSKid = char.GetPetSkill( petindex, i);
					if TMSKid >= 0 then
						PetSK[i+1] = petskill.getChar( TMSKid, "����"); 
					end
				end
				local PetE1 = "";
				for i=1,7 do
					if i == 7 then
						PetE1 = PetE1 .. PetSK[i];
					else
						PetE1 = PetE1 .. PetSK[i] .. "|";
					end
				end
				local retstr = PetA1.."|"..PetA2.."|"..PetA3.."|"..PetA4.."|"..PetA5.."\n"
							.. PetB1.."|"..PetB2.."|"..PetB3.."|"..PetB4.."|"..PetB5.."\n"
							.. PetC1.."|"..PetC2.."|"..PetC3.."|"..PetC4.."\n"
							.. PetD1.."|"..PetD2.."|"..PetD3.."|"..PetD4.."\n"
							.. PetE1.."\n";	
				--print(retstr);
				lssproto.windows(charaindex, 1016, 12, -1, char.getWorkInt( npcindex, "����"), retstr);
			else
				char.TalkToCli(charaindex, -1, "�ó����Ѿ����߻��߲������ˣ�", "��ɫ");
			end
		elseif index2 == 2 then--����
			print(index3);
		end
	else
		print(index1,index2,index3);
		char.TalkToCli(charaindex, -1, "��δ����", "��ɫ")
	end
end

function WindowTalked( meindex, talkerindex, seqno, select, data)
	if seqno == 0 then
		local num = tonumber(data);
		if num > 0 and num < 5 then
			if char.getInt(partytable[talkerindex][num],"����") ~= 1 then
				char.TalkToCli(talkerindex, -1, "�ӳ�ֻ��ת�ø���ҡ�", "��ɫ")
				return
			end
			char.DischargeParty(talkerindex,0);
			char.JoinParty(partytable[talkerindex][num],talkerindex,0);
			char.TalkToCli(partytable[talkerindex][num], -1, "���Ķ��ѡ�"..char.getChar(talkerindex,"����").."���Ѷӳ�Ȩ����������", "��ɫ")
			char.TalkToCli(talkerindex, -1, "���ɹ��Ѷӳ�Ȩ���������ѡ�"..char.getChar(partytable[talkerindex][num],"����").."��", "��ɫ");
			for i =1,table.getn(partytable[talkerindex]) do
				if i ~= num then
					char.JoinParty(partytable[talkerindex][num],partytable[talkerindex][i],0);
				end
			end
			char.Encounter(partytable[talkerindex][num]);
		end
	end
end

function Create(name, metamo, floor, x, y, dir)
	npcindex = npc.CreateNpc(name, metamo, floor, x, y, dir)
	char.setFunctionPointer(npcindex, "�����¼�", "WindowTalked", "")
end

function mydata()
	partytable={};

end

function main()
	mydata();
	Create("samenu����", 100000, 777, 32, 18, 6);
end

