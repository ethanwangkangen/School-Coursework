const MAX_USERS: usize = 1000;
const MAX_NAME_LEN: usize = 50;
const MAX_EMAIL_LEN: usize = 50;
const MAX_PASSWORD_LENGTH: usize = 100;
const INACTIVITY_THRESHOLD: i32 = 5;
const MAX_SESSION_TOKEN_LEN: usize = 32;


#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum OwnershipType {
    C = 0,
    Rust = 1,
}

#[derive(Debug, Clone)]
#[repr(C)]
pub struct UserStruct {
    pub password: [u8; MAX_PASSWORD_LENGTH],
    pub username: [u8; MAX_NAME_LEN],
    pub user_id: i32,
    pub email: [u8; MAX_EMAIL_LEN],
    pub inactivity_count: i32,
    pub is_active: i32,
    pub session_token: [u8; MAX_SESSION_TOKEN_LEN],
    pub shared: i32,
    pub owner: OwnershipType,

}

impl Default for UserStruct {
    fn default() -> Self {
        UserStruct {
            password: [0; MAX_PASSWORD_LENGTH],
            user_id: 0,
            email: [0; MAX_EMAIL_LEN],
            inactivity_count: 0,
            username: [0; MAX_NAME_LEN],
            session_token: [0; MAX_SESSION_TOKEN_LEN],
            is_active: 0,
            shared: 0,
            owner: OwnershipType::Rust,
        }
    }
}

#[derive(Debug)]
pub struct UserDatabase {
    pub users: [Option<Box<UserStruct>>; MAX_USERS],
    pub count: i32,
    pub capacity: i32,
}

use std::str;
use std::cmp::min;

// Helper function.
// Takes in a mutable reference to an array, and returns a String
pub fn array_to_string<const N: usize>(bytes: &[u8; N]) -> String {
    //match str::from_utf8(bytes) {
    //    Ok(s) => s.trim_end_matches('\x00').to_string(),
    //    Err(e) => format!("Error: {}", e),
    //}
    let len = bytes.iter().position(|&b| b == 0).unwrap_or(N);
    String::from_utf8_lossy(&bytes[..len]).to_string()
}

pub fn init_database() -> Box<UserDatabase> {
  let db = UserDatabase {
    users: std::array::from_fn(|_| None),
    count: 0,
    capacity: MAX_USERS as i32,
  };

  Box::new(db)
}

//pub fn add_user(db: &mut UserDatabase, user: Box<UserStruct>) {
//  if (db.count as usize)< MAX_USERS as usize{
    
//    if user.owner == OwnershipType::C {
    //if false{
            // C owns this memory: don't wrap in Box
            //let raw: *mut UserStruct = Box::into_raw(user); // get the pointer
            //db.users[db.count as usize] = Some(unsafe { Box::from_raw(raw) });
            // Now immediately forget, so Rust won’t free later
            //std::mem::forget(raw); 
            

        // C owns this memory: don't give Rust responsibility to free
//    let ptr: *mut UserStruct = Box::into_raw(user);

    // Leak the Box permanently to stop Rust from freeing
    // This makes the allocation "immortal" from Rust's perspective.
//    let leaked_ref: &'static mut UserStruct = unsafe { &mut *ptr };

    // Re-wrap the leaked reference in a Box. This Box points to a leaked value,
    // so when dropped later it won't try to free C's memory.
//    let fake_box: Box<UserStruct> = unsafe { Box::from_raw(leaked_ref) };
//        db.users[db.count as usize] = Some(fake_box);

//        } else {
//            let mut user_mut : Box<UserStruct> = user;
//            user_mut.user_id = db.count;
//            db.users[db.count as usize] = Some(user_mut); // Rust-owned
//        }
        
//    db.count +=1;
//  }
//}

pub fn add_user(db: &mut UserDatabase, user: Box<UserStruct>) {
    if (db.count as usize) < MAX_USERS {
        if user.owner == OwnershipType::C {
            // Convert Box<UserStruct> into raw pointer
            //let ptr: *mut UserStruct = Box::into_raw(user);

            let leaked_ref: &'static mut UserStruct = Box::leak(user);
            // Immediately wrap back in a Box so we can put it into db.users
            let fake_box: Box<UserStruct> = unsafe { Box::from_raw(leaked_ref) };

            // Store in the DB
            db.users[db.count as usize] = Some(fake_box);
            if let Some(ref u) = db.users[db.count as usize] {
                std::mem::forget(u);
            }
        } else {
            let mut user_mut = user;
            user_mut.user_id = db.count;
            db.users[db.count as usize] = Some(user_mut); // Rust-owned
        }

        db.count += 1;
    }
}


fn free_user(user : &mut UserStruct){
}


pub fn print_database(db: &UserDatabase) {
 for i in 0..db.count {
    // 'if let' is a pattern matching expression
    //
    // Here, it tries to match &db.users[i] (a reference to an Option<Box<UserStruct>>)
    // against the pattern Some(user).
    //
    // If the slot contains Some, the block runs and'user' is a reference to the Box<UserStruct>.
    // If it's None, the block is skipped.

    if let Some(user) = &db.users[i as usize] {
      println!(
        "User: {}, ID: {}, Email: {}, Inactivity: {}, Password: {}",
        array_to_string(&user.username),
        user.user_id,
        array_to_string(&user.email),
        user.inactivity_count,
        array_to_string(&user.password),
      );
    }
  }
}


fn copy_string(dest : &mut [u8], src : &str, n : usize) {
    let bytes = src.as_bytes();

    let max = dest.len() -1;
    let copyLen = min(n, min(bytes.len(), max));

    for i in 0..copyLen {
        dest[i] = bytes[i];
    }
    dest[copyLen] = 0; //null terminate
}

pub fn create_user(username: &str, email: &str, user_id: i32, password: &str) -> Box<UserStruct> {
    let mut user = UserStruct {
        password: [0; MAX_PASSWORD_LENGTH],
        username: [0; MAX_NAME_LEN],
        user_id,
        email: [0; MAX_EMAIL_LEN],
        inactivity_count: 0,
        is_active : 0,
        session_token: [0; MAX_SESSION_TOKEN_LEN],
        shared:0,
        owner:OwnershipType::Rust,
    };

    copy_string(&mut user.password, password, password.len());
    copy_string(&mut user.username, username, username.len());
    copy_string(&mut user.email, email, email.len());
    Box::new(user) // Return the user
}

pub fn find_user_by_id(db: &mut UserDatabase, user_id: i32) -> Option<&mut UserStruct> {
    // .iter_mut() to bypass multiple borrowing issues when iterating via for i in 0..db.count
    for user_option in db.users.iter_mut() {
        // db.users is [Option<Box<UserStruct>>; MAX_USERS]
        // iter_mut() produces an iterator over this

        // user_option has type: &mut Option<Box<UserStruct>>
        if let Some(user) = user_option {
            // user is now &mut <Box<UserStruct>>

            if user.user_id == user_id {
                
                // First dereference: get Box<UserStruct>
                // Second dereference: get UserStruct
                // Add in &mut: get &mut UserStruct
                // Add in Some(): get Option<&mut UserStruct>
                return Some(&mut **user); 

            }
        }
    }
    None
}

pub fn cleanup_database(db : &mut UserDatabase) {
    for i in 0..db.count {
        if let Some(user) = &db.users[i as usize] {
            if user.owner == OwnershipType::C {
                continue;
            }
            db.users[i as usize] = None;
        }

    }
}

pub fn update_database_daily(db: &mut UserDatabase) {
    for i in 0..db.count {
        if let Some(user) = &mut db.users[i as usize] { //mutable borrow
            if user.inactivity_count > INACTIVITY_THRESHOLD {
                if user.owner == OwnershipType::C {
                    continue;
                }
                println!("[Rust] Removing user {} for inactivity", array_to_string(&user.username));
                db.users[i as usize] = None;
                db.count-=1;
                //free_user(user);
            } else {
                if user.owner == OwnershipType::C {
                    continue;
                }
                println!("[Rust] Incrementing inactivity count for {}", array_to_string(&user.username));
                user.inactivity_count +=1 ;
            }
        }
    }
}



/*
pub fn update_database_daily(db: &mut UserDatabase) {
    for i in 0..db.count {
        if let Some(mut user_box) = db.users[i as usize].take() {
            if user_box.inactivity_count > INACTIVITY_THRESHOLD {
                if user_box.owner == OwnershipType::C {
                    // Leak so Rust won’t free C-owned memory
                    println!("[Rust] Leaking C owned user {} for inactivity",
                             array_to_string(&user_box.username));
                    //let _ = Box::into_raw(user_box);
                } else {
                    println!(
                        "[Rust] Removing Rust owned user {} for inactivity",
                        array_to_string(&user_box.username)
                    );
                    // Rust-owned: dropping user_box here frees the memory
                }
                db.users[i as usize] = None;
                db.count -= 1;
            } else {
                if user_box.owner != OwnershipType::C {
                    println!(
                        "[Rust] Incrementing inactivity count for {}",
                        array_to_string(&user_box.username)
                    );
                    user_box.inactivity_count += 1;
                }
                // Put it back
                db.users[i as usize] = Some(user_box);
            }
        }
    }
}
*/


pub fn update_username (db: &mut UserDatabase, username : &str, new_username : &str) {
    if let Some(user) = find_user_by_username_mut(db, username) {
        // user is &mut UserStruct

        if new_username.len() >= 50 {
            let truncated: String = new_username.chars().take(49).collect();
            copy_string(&mut user.username, &truncated, truncated.len());
        } else {
           copy_string(&mut user.username, new_username, new_username.len());
        }
    }
}

pub fn user_login(db: &mut UserDatabase, username: &str) {
    if let Some(user) = find_user_by_username_mut(db, username) {
        //user is &mut UserStruct
        user.inactivity_count = 0;
    }
}

pub fn get_password(db : &mut UserDatabase, username: &str) -> Option<String> {
    if let Some(user) = find_user_by_username_mut(db, username) {
        //user is &mut UserStruct
        Some(array_to_string(&user.password))
    } else {
        None
    }
}

fn update_password(db : &mut UserDatabase, username: &str , password: &str) {
    if let Some(user) = find_user_by_username_mut(db, username) {
        //user is &mut UserStruct
        copy_string(&mut user.password, password, password.len());
    }  
}

fn print_user(db : &mut UserDatabase, username : &str) {
    if let Some(user) = find_user_by_username_mut(db, username) {
        //user is a &mut UserStruct
        println!("User[{}] {}: Email: {}, Inactivity: {}, Password: {} \n",
                 user.user_id,
                 array_to_string(&user.username),
                 array_to_string(&user.email),
                 user.inactivity_count,
                 array_to_string(&user.password)
                 );
    } 

}

pub fn find_user_by_username<'a>(db : & 'a UserDatabase, username: &'a str) -> Option<&'a UserStruct> {
    for user_option in db.users.iter() {
        if let Some(user) = user_option {
            if std::str::from_utf8(&user.username)
                .unwrap_or("")
                .trim_end_matches(char::from(0)) == username
                {
                    return Some(& **user);
                }
        }
    } 
    None

}

pub fn find_user_by_username_mut<'a>(db: & 'a mut UserDatabase, username: &'a str) -> Option<&'a mut UserStruct> {
    for user_option in db.users.iter_mut() {
        if let Some(user) = user_option{
        
            if std::str::from_utf8(&user.username)
                .unwrap_or("")
                .trim_end_matches(char::from(0)) == username
                {
                    return Some(&mut **user);
                }
        }
    } None
}




fn main() {
    print!("Hello, world!");
}
