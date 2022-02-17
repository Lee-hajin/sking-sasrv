--经验计算
function FreeMagicMpRate( charaindex, marray, mp)
	--print( charaindex, marray, mp);
	if char.getInt( charaindex, "家族地位") > 0 and char.getInt( charaindex, "家族地位") ~= 2 then
		if char.getInt( charaindex, "家族类型") == 0 then--光明家族
			if marray >= 0 and marray < 130 then
				mp = mp * 0.6;
			end
		elseif char.getInt( charaindex, "家族类型") == 1 then--黑暗家族
			if marray >= 130 and marray < 190 then--黑暗精灵
				mp = mp * 0.6;
			end
		end
	end
	return mp;
end

function mydata()
	
end

function main()
	mydata();
end
