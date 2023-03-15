set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(target_put_into_folder targetName folder_path)
    message(NOTICE "Put [${targetName}]\tinto \"${folder_path}\"")
    set_target_properties(${targetName} PROPERTIES FOLDER ${folder_path})
endfunction(target_put_into_folder)