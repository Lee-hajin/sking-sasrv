--丢弃道具事件，返回0为不可丢弃，返回1为可丢弃
function FreeDropItem( charaindex, itemindex )
	--print("调试输出:",charaindex,itemindex);
	return 1;
end

--丢弃宠物事件
function FreeDropPet( charaindex, havepetindex )
	local PetIndex = char.getCharPet(charaindex, havepetindex);--索引
	if char.getInt(PetIndex,"绑定") == 1 then
		local token = "绑定的宠物丢弃会直接消失哦！\n"
					.."你确定要丢弃【"..char.getChar(PetIndex,"名字").."】吗？";
		lssproto.windows(charaindex, 0, 12, havepetindex, char.getWorkInt( npcindex, "对象"), token);
		return 0;
	end
	return 1;
end

function WindowTalked ( meindex, talkerindex, seqno, select, data)
	if seqno >= 0 and seqno <= 4 then
		if select == 4 then
			local petindex = char.getCharPet(talkerindex,seqno);
			if char.check(petindex) ~= 1 then
				return ;
			end
			if char.getInt(petindex,"绑定") == 1 then
				char.TalkToCli(talkerindex, -1, "您成功丢掉了["..char.getChar(petindex,"名字").."]["..char.getInt(petindex,"等级").."](绑定宠物，丢弃直接消失)", 4);
				char.DelPet(talkerindex, petindex);
				
			else
				char.DropPet(talkerindex,seqno);
			end
		end
	end
end

function mydata()
					 
end

function main()
	mydata();
	npcindex = npc.CreateNpc("checkdrop窗口", 100001, 777, 32, 16, 6);
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
	
end
