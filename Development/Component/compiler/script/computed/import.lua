return function (path_in_archive)
	return function (buf, is_path)		
		if is_path then
			log.trace("[stormlib]import file", path_in_archive)
			fs.copy_file(__map_path__:parent_path() / buf, __map_path__ / path_in_archive, true)
			return
		else
			local temp_file_path = fs.ydwe_path() / "logs" / "import" / path_in_archive
			fs.create_directories(temp_file_path:parent_path())
			if not io.save(temp_file_path, buf) then
				log.error("failed: save " .. temp_file_path:string())
				return
			end
			log.trace("[stormlib]import file", path_in_archive)
			fs.create_directories((__map_path__ / path_in_archive):parent_path())
			fs.copy_file(temp_file_path, __map_path__ / path_in_archive, true)
			return
		end
	end
end
