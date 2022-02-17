function FreeBattleCommand( battleindex, charaindex, side)
	if char.getInt(charaindex, "类型") == 3 then
		local myindex = char.getWorkInt(charaindex,"宠物主人索引")
		if char.check(myindex) == 1 then
			if char.getWorkInt(myindex,"离线") > 0 and char.getFlg(myindex,"死亡") == 1 then
				offline.Guard(charaindex)
				return
			end
		end
	end
end
